#pragma once


#include <stdint.h>
#include <memory>
#include <fstream>
#include <SFML/Graphics.hpp>

#define DEBUG

enum 
{
	RUNNING,
	PAUSE,
	QUIT
};

// sfml variables
struct sfml_t
{
	sf::RenderWindow window;

};

// Configuration for the display
struct config_t
{
	uint32_t screen_width;
	uint32_t screen_height;
	uint32_t scale_factor;
	uint32_t fg_color;
	uint32_t bg_color;
	const char* rom_name;
};

struct registers_t
{
	uint16_t I;		 // index register
	uint8_t V[0x10]; // V0-VF registers

};

struct instruction_t
{
	uint16_t opcode;
	uint16_t NNN;    // 12 bit address/constant
	uint8_t  NN;     // 8 bit constant
	uint8_t  N;      // 4 bit constant
	uint8_t  X;      // 4 bit register identifier
	uint8_t  Y;      // 4 bit register identifier
};

// The chip8's internals
struct chip8_t
{
	uint8_t ram[4096];		// 4kb of RAM
	bool display[64 * 32];	// display pixels (1 = Pixel on, 0 = Pixel off)
	//bool display[64][32];
	uint16_t stack[12];	// 42 Bytes of stack memory 
	uint16_t* stack_ptr;
	uint32_t PC;
	instruction_t inst;		// Currently running opcode
	registers_t regs;
	uint8_t status;
	bool draw;
};

void init_config(config_t& config)
{
	// Default settings
	config =
	{
		.screen_width = 64,
		.screen_height = 32,
		.scale_factor = 20,
		.fg_color = 0xFF0000FF, // white
		.bg_color = 0x00000000, // black
		.rom_name = "ROM/5-quirks.ch8"
	};
	
}

bool init_sfml(sfml_t& sfml, const config_t& config)
{
	uint16_t width  = config.screen_width * config.scale_factor;
	uint16_t height = config.screen_height * config.scale_factor;

	sfml.window.create(sf::VideoMode(width, height), "CHIP-8");
	
	

	return true;
}


bool init_chip8(chip8_t& chip8, config_t config)
{
	const uint32_t entry_point = 0x200; // CHIP8 Roms will be loaded to 0x200
	const uint8_t font[] =
	{
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};


	// Load font
	memcpy(&chip8.ram[0], font, sizeof(font));

	// Load ROM into memory
	
	// Clear all registers
	for (size_t i = 0; i < 0xF; i++)
	{
		chip8.regs.V[i] = 0;
	}

	chip8.regs.I = 0;

	const char* rom_name = config.rom_name;
	

	// Open ROM file
	FILE* rom = nullptr;
	errno_t err = fopen_s(&rom, rom_name, "rb");
	if (err != 0) {
		printf("Rom file %s is invalid or does not exist\n", rom_name);
		return false;
	}

	// Get/check rom size
	fseek(rom, 0, SEEK_END);
	const size_t rom_size = ftell(rom);
	const size_t max_size = sizeof(chip8.ram) - entry_point;
	rewind(rom);

	if (rom_size > max_size) {
		printf("Rom file %s is too big! Rom size: %llu, Max size allowed: %llu\n",
		rom_name, (long long unsigned)rom_size, (long long unsigned)max_size);
		return false;
	}

	// Load ROM
	if (fread(&chip8.ram[entry_point], rom_size, 1, rom) != 1) {
		printf("Could not read Rom file %s into CHIP8 memory\n",
			rom_name);
		return false;
	}
	// Close ROM
	fclose(rom);

	

	// Clear Display buffer
	memset(&chip8.display[0], false, sizeof(chip8.display));

	// Set Program Counter to correct location at start
	chip8.PC = entry_point;		// location of where programs expect PC to be at 

	// Set up stack ptr
	chip8.stack_ptr = &chip8.stack[0];

	chip8.status = RUNNING;

	return true;

}

void clear_screen(sfml_t& sfml, const config_t& config)
{
	const uint8_t bg_r = (config.bg_color >> 24) & 0xFF;
	const uint8_t bg_g = (config.bg_color >> 16) & 0xFF;
	const uint8_t bg_b = (config.bg_color >> 8) & 0xFF;
	const uint8_t bg_a = (config.bg_color) & 0xFF;

	sfml.window.clear(sf::Color(bg_r, bg_g, bg_b, bg_a));
	sfml.window.display();
}

void update_screen(sfml_t& sfml, const config_t& config, chip8_t& chip8)
{
	const uint8_t bg_r = (config.bg_color >> 24) & 0xFF;
	const uint8_t bg_g = (config.bg_color >> 16) & 0xFF;
	const uint8_t bg_b = (config.bg_color >> 8)  & 0xFF;
	const uint8_t bg_a = (config.bg_color)       & 0xFF;

	sfml.window.clear(sf::Color(bg_r, bg_g, bg_b, bg_a));

	const uint8_t fg_r = (config.fg_color >> 24) & 0xFF;
	const uint8_t fg_g = (config.fg_color >> 16) & 0xFF;
	const uint8_t fg_b = (config.fg_color >> 8)  & 0xFF;
	const uint8_t fg_a = (config.fg_color)	     & 0xFF;

	sf::RectangleShape pixel(sf::Vector2f(config.scale_factor, config.scale_factor));
	pixel.setFillColor(sf::Color(fg_r, fg_g, fg_b, fg_a));
	
	
	for (int X = 0; X < config.screen_width; X++)
	{
		for (int Y = 0; Y < config.screen_height; Y++)
		{
			if (chip8.display[Y * config.screen_width + X] == true)
			{
				pixel.setPosition(X * config.scale_factor, Y * config.scale_factor);
				sfml.window.draw(pixel);
			}
		}
	}
	

	sfml.window.display();
}

void user_input(sfml_t& sfml, chip8_t& chip8)
{
	sf::Event event;
	while (sfml.window.pollEvent(event))
	{
		/* Event Loop */
		switch (event.type)
		{
		case sf::Event::Closed:
			sfml.window.close();
			break;

		case sf::Event::KeyPressed:
			
			/* Key press Loop */

			switch (event.key.code)
			{
			case sf::Keyboard::Escape:
				sfml.window.close();
				break;

			case sf::Keyboard::Space:
				chip8.status = PAUSE;
				
				break;

			default:
				break;
			}

			break;

		

		default:
			break;
		}

	}
}

void run_single_opcode_d(chip8_t chip8, const config_t config)
{
	bool carry;

	switch ((chip8.inst.opcode >> 12) & 0xF)
	{
	// 1NNN: Jump to address
	case 0x1:
		printf("<0x%X> PC = 0x%X:  Jumping to address 0x%X\n", chip8.inst.opcode, chip8.PC - 2, chip8.inst.NNN);
		break;
		
	// 2NNN: Call to address and put return address on stack
	case 0x2:
		printf("<0x%X> PC = 0x%X:  Calling to address 0x%X and pushing to stack at 0x%X\n",
			chip8.inst.opcode, 
			chip8.PC - 2,
			chip8.inst.NNN,
			chip8.stack_ptr - 1);
		break;

	// 3XNN: if (Vx == NN)
	case 0x3:
		printf("<0x%X> PC = 0x%X:  if (V%X == 0x%X)\n", 
			chip8.inst.opcode, 
			chip8.PC - 2, 
			chip8.regs.V[chip8.inst.X], 
			chip8.inst.NN);
		break;

	// 4XNN: if (Vx != NN)
	case 0x4:
		printf("<0x%X> PC = 0x%X:  if (V%X != 0x%X)\n", 
			chip8.inst.opcode, 
			chip8.PC - 2, 
			chip8.regs.V[chip8.inst.X], 
			chip8.inst.NN);
		break;

	// 5XNN: if (Vx == Vy)
	case 0x5:
		printf("<0x%X> PC = 0x%X:  if (V%X == 0x%X)\n", 
			chip8.inst.opcode, 
			chip8.PC - 2, 
			chip8.regs.V[chip8.inst.X], 
			chip8.regs.V[chip8.inst.Y]);
		break;
		
	// 6XNN: Vx = NN
	case 0x6:
		printf("<0x%X> PC = 0x%X:  V%X = 0x%X\n", chip8.inst.opcode, chip8.PC - 2, chip8.inst.X, chip8.inst.NN);
		break;

	// 7XNN: Vx += NN
	case 0x7:
		printf("<0x%X> PC = 0x%X:  V%X += 0x%X (V%d = 0x%X)\n",
			chip8.inst.opcode, 
			chip8.PC - 2,  
			chip8.inst.X, 
			chip8.inst.NN, 
			chip8.inst.X, 
			chip8.regs.V[chip8.inst.X]);
		break;

		// 8XY0/1/2/3/4/5/6/7/E
	case 0x8:
		switch (chip8.inst.N)
		{

			// Vx = Vy
		case 0x0:
			printf("<0x%X> PC = 0x%X:  V%X = 0x%X\n",
				chip8.inst.opcode,
				chip8.PC - 2,
				chip8.regs.V[chip8.inst.X],
				chip8.regs.V[chip8.inst.Y]);
			break;
			// Vx |= Vy
		case 0x1:
			printf("<0x%X> PC = 0x%X:  V%X |= 0x%X\n",
				chip8.inst.opcode,
				chip8.PC - 2,
				chip8.regs.V[chip8.inst.X],
				chip8.regs.V[chip8.inst.Y]);
			break;

			// Vx &= Vy
		case 0x2:
			printf("<0x%X> PC = 0x%X:  V%X &= 0x%X\n",
				chip8.inst.opcode,
				chip8.PC - 2,
				chip8.regs.V[chip8.inst.X],
				chip8.regs.V[chip8.inst.Y]);
			break;

			// Vx ^= Vy
		case 0x3:
			printf("<0x%X> PC = 0x%X:  V%X ^= 0x%X\n",
				chip8.inst.opcode,
				chip8.PC - 2,
				chip8.regs.V[chip8.inst.X],
				chip8.regs.V[chip8.inst.Y]);
			break;

			// Vx += Vy
		case 0x4:
			printf("<0x%X> PC = 0x%X:  V%X += 0x%X \n",
				chip8.inst.opcode,
				chip8.PC - 2,
				chip8.regs.V[chip8.inst.X],
				chip8.regs.V[chip8.inst.Y]);
			break;

			// Vx -= Vy
		case 0x5:
			printf("<0x%X> PC = 0x%X:  V%X -= 0x%X \n",
				chip8.inst.opcode,
				chip8.PC - 2,
				chip8.regs.V[chip8.inst.X],
				chip8.regs.V[chip8.inst.Y]);
			break;

			// Vx >>= 1
		case 0x6:
			printf("<0x%X> PC = 0x%X:  V%X >>= 1 \n",
				chip8.inst.opcode,
				chip8.PC - 2,
				chip8.regs.V[chip8.inst.X]);
			break;

			// Vy -= Vx
		case 0x7:
			printf("<0x%X> PC = 0x%X:  V%X -= 0x%X \n",
				chip8.inst.opcode,
				chip8.PC - 2,
				chip8.regs.V[chip8.inst.Y],
				chip8.regs.V[chip8.inst.X]);
			break;

			// Vx <<= 1
		case 0xE:
			printf("<0x%X> PC = 0x%X:  V%X <<= 1 \n",
				chip8.inst.opcode,
				chip8.PC - 2,
				chip8.regs.V[chip8.inst.X]);
			break;
		}


	// ANNN: I = NNN
	case 0xA:
		printf("<0x%X> PC = 0x%X:  I = 0x%X\n", chip8.inst.opcode, chip8.PC - 2, chip8.inst.NNN);
		break;

	case 0xB:
		printf("PC = NNN + V0");
		break;

		/* DXYN:
		  *  It will draw an N pixels tall sprite from the memory location that the I index register is holding to the screen,
		  *  at the horizontal X coordinate in VX and the Y coordinate in VY.
		  *  All the pixels that are “on” in the sprite will flip the pixels on the screen that it is drawn to (from left to right, from most to least significant bit).
		  *  If any pixels on the screen were turned “off” by this, the VF flag register is set to 1. Otherwise, it’s set to 0.
		*/
	case 0xD:
		printf("<0x%X> PC = 0x%X:  Displaying pixel at address 0x%X (X, Y = %d, %d) of %d rows\n",chip8.inst.opcode, 
			chip8.PC - 2, 
			chip8.regs.I,
			chip8.regs.V[chip8.inst.X],
			chip8.regs.V[chip8.inst.Y],
			chip8.inst.N);
		
		break;

	default:
		printf("<0x%X> PC = 0x%X:  Unimpletmented opcode 0x%X\n", chip8.inst.opcode, chip8.PC - 2, chip8.inst.opcode);
		break;
	}
}

void run_single_opcode(chip8_t& chip8, const config_t& config, sfml_t& sfml)
{
	
	chip8.inst.opcode = (chip8.ram[chip8.PC] << 8) | (chip8.ram[chip8.PC + 1]);
	chip8.PC += 2; // 16 bit instructions so add 2

	chip8.inst.NNN = (chip8.inst.opcode)  	  & 0xFFF;	// Address
	chip8.inst.NN  = (chip8.inst.opcode)	  & 0xFF;	// 8-bit constant
	chip8.inst.N   = (chip8.inst.opcode)  	  & 0xF;	// 4-bit constant
	chip8.inst.X   = (chip8.inst.opcode >> 8) & 0xF;	// 4-bit register identifier
	chip8.inst.Y   = (chip8.inst.opcode >> 4) & 0xF;	// 4-bit register identifier

#ifdef DEBUG
	run_single_opcode_d(chip8, config);
#endif // DEBUG


	uint32_t X_coord;		// Used for DXYN 
	uint32_t Y_coord;		// Used for DXYN
	uint8_t  orig_X;		// Used for DXYN
	bool carry;

	switch ((chip8.inst.opcode >> 12) & 0xF)
	{
	// 00E0: Clear Screen/Return from call
	case 0x0:
		if (chip8.inst.NN == 0xE0)
			clear_screen(sfml, config);
		
		if (chip8.inst.NN == 0xEE)
			chip8.PC = *--chip8.stack_ptr;

		break;

	// 1NNN: Jump to address
	case 0x1:
		chip8.PC = chip8.inst.NNN;
		break;

	// 2NNN: Calls to Address (Pushes return to stack
	case 0x2:
		*chip8.stack_ptr++ = chip8.PC;
		chip8.PC = chip8.inst.NNN;
		break;

	// 3XNN: if (Vx == NN)
	case 0x3:
		if (chip8.regs.V[chip8.inst.X] == chip8.inst.NN)
			chip8.PC += 2;
		break;

	// 4XNN: if (Vx != NN)
	case 0x4:
		if (chip8.regs.V[chip8.inst.X] != chip8.inst.NN)
			chip8.PC += 2;
		break;

	// 5XNN: if (Vx == Vy)
	case 0x5:
		if (chip8.regs.V[chip8.inst.X] == chip8.regs.V[chip8.inst.Y])
			chip8.PC += 2;
		break;

	// 6XNN: Vx = NN
	case 0x6:
		chip8.regs.V[chip8.inst.X] = chip8.inst.NN;
		break;

	// 7XNN: Vx += NN
	case 0x7:
		chip8.regs.V[chip8.inst.X] += chip8.inst.NN;

		break;

	// 8XY0/1/2/3/4/5/6/7/E
	case 0x8:
		switch (chip8.inst.N)
		{

		// Vx = Vy
		case 0x0:
			chip8.regs.V[chip8.inst.X] = chip8.regs.V[chip8.inst.Y];
			break;
		// Vx |= Vy
		case 0x1:
			chip8.regs.V[chip8.inst.X] |= chip8.regs.V[chip8.inst.Y];
			break;

		// Vx &= Vy
		case 0x2:
			chip8.regs.V[chip8.inst.X] &= chip8.regs.V[chip8.inst.Y];
			break;

		// Vx ^= Vy
		case 0x3:
			chip8.regs.V[chip8.inst.X] ^= chip8.regs.V[chip8.inst.Y];
			break;

		// Vx += Vy
		case 0x4:
			carry = ((uint16_t)(chip8.regs.V[chip8.inst.X] + chip8.regs.V[chip8.inst.Y]) > 255);

			chip8.regs.V[chip8.inst.X] += chip8.regs.V[chip8.inst.Y];
			chip8.regs.V[0xF] = carry;
			break;

		// Vx -= Vy
		case 0x5:
			carry = chip8.regs.V[chip8.inst.X] > chip8.regs.V[chip8.inst.Y];

			chip8.regs.V[chip8.inst.X] -= chip8.regs.V[chip8.inst.Y];
			chip8.regs.V[0xF] = carry;
			break;

		// Vx >>= 1
		case 0x6:
			carry = (chip8.regs.V[chip8.inst.X] & 0x80) << 1;  // VX
			chip8.regs.V[chip8.inst.X] >>= 1;                  // Use VX
			chip8.regs.V[0xF] = carry;
			break;

		// Vy -= Vx
		case 0x7:
			carry = chip8.regs.V[chip8.inst.Y] > chip8.regs.V[chip8.inst.X];

			chip8.regs.V[chip8.inst.Y] -= chip8.regs.V[chip8.inst.X];
			chip8.regs.V[0xF] = carry;
			break;

		// Vx <<= 1
		case 0xE:
			carry = (chip8.regs.V[chip8.inst.X] & 0x80) >> 7;  // VX
			chip8.regs.V[chip8.inst.X] <<= 1;                  // Use VX
			chip8.regs.V[0xF] = carry;
			break;
		}


	// 9XNN: if (Vx != Vy)
	case 0x9:
		if (chip8.regs.V[chip8.inst.X] != chip8.regs.V[chip8.inst.Y])
			chip8.PC += 2;
		break;

	// ANNN: I = NNN
	case 0xA:
		chip8.regs.I = chip8.inst.NNN;
		break;

	// BXNN: PC = Vx += NN; (etc B220 = 0x220 + V0)
	case 0xB:
		// COSMAC VIP
		chip8.PC = chip8.regs.V[0] + chip8.inst.NNN;
		break;

	// 0xCXNN: Sets register VX = rand() % 256 & NN (bitwise AND)
	case 0xC:
		chip8.regs.V[chip8.inst.X] = (rand() % 256) & chip8.inst.NN;
		break;

	/* DXYN:
	  *  It will draw an N pixels tall sprite from the memory location that the I index register is holding to the screen,
	  *  at the horizontal X coordinate in VX and the Y coordinate in VY. 
	  *  All the pixels that are “on” in the sprite will flip the pixels on the screen that it is drawn to (from left to right, from most to least significant bit). 
	  *  If any pixels on the screen were turned “off” by this, the VF flag register is set to 1. Otherwise, it’s set to 0.
	*/
	case 0xD:
		
		// Wrap around the screen if pixel goes out of bounds
		X_coord = chip8.regs.V[chip8.inst.X] % config.screen_width;
		Y_coord = chip8.regs.V[chip8.inst.Y] % config.screen_height;
		orig_X = X_coord; // Original X value

		chip8.regs.V[0xF] = 0;	// Set carry to 0

		

		// Read sprite from memory
		// Loop over all N rows of the sprite
		for (uint8_t i = 0; i < chip8.inst.N; i++) 
		{
			// Get next byte/row of sprite data
			const uint8_t sprite_data = chip8.ram[chip8.regs.I + i];
			X_coord = orig_X;   // Reset X for next row to draw

			for (int8_t j = 7; j >= 0; j--) 
			{
				// If sprite pixel/bit is on and display pixel is on, set carry flag
				bool* pixel = &chip8.display[Y_coord * config.screen_width + X_coord];
				const bool sprite_bit = (sprite_data & (1 << j));

				if (sprite_bit && *pixel) 
				{
					chip8.regs.V[0xF] = 1;
				}

				// XOR display pixel with sprite pixel/bit to set it on or off
				*pixel ^= sprite_bit;

				;

				// Stop drawing this row if hit right edge of screen
				if (++X_coord >= config.screen_width) break;
			}

			

			// Stop drawing entire sprite if hit bottom edge of screen
			if (++Y_coord >= config.screen_height) break;
		}

		chip8.draw = true;

		break;

	
	default:
		break;
	}
}
