#pragma once


#include <stdint.h>
#include <memory>
#include <fstream>
#include <SFML/Graphics.hpp>

#include "structs.hpp"
#include "init.hpp"




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

// Handle user input
// CHIP8 Keypad
// QWERTY keyboard
/*
*  CHIP8	   QWERTY
* 
*	123C		1234
*	456D		QWER
*	789E		ASDF
*	A0BF		ZXCV
*/

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

			/* Map qwerty keys to CHIP8 keypad */
			case sf::Keyboard::Num1: chip8.keypad[0x1] = true; break;
			case sf::Keyboard::Num2: chip8.keypad[0x2] = true; break;
			case sf::Keyboard::Num3: chip8.keypad[0x3] = true; break;
			case sf::Keyboard::Num4: chip8.keypad[0xC] = true; break;
			
			case sf::Keyboard::Q:	 chip8.keypad[0x4] = true; break;
			case sf::Keyboard::W:	 chip8.keypad[0x5] = true; break;
			case sf::Keyboard::E:	 chip8.keypad[0x6] = true; break;
			case sf::Keyboard::R:	 chip8.keypad[0xD] = true; break;
			
			case sf::Keyboard::A:	 chip8.keypad[0x7] = true; break;
			case sf::Keyboard::S:	 chip8.keypad[0x8] = true; break;
			case sf::Keyboard::D:	 chip8.keypad[0x9] = true; break;
			case sf::Keyboard::F:	 chip8.keypad[0xE] = true; break;

			case sf::Keyboard::Z:	 chip8.keypad[0xA] = true; break;
			case sf::Keyboard::X:	 chip8.keypad[0x0] = true; break;
			case sf::Keyboard::C:	 chip8.keypad[0xB] = true; break;
			case sf::Keyboard::V:	 chip8.keypad[0xF] = true; break;

			default:
				break;
			}

		case sf::Event::KeyReleased:
			switch (event.key.code)
			{

			/* Map qwerty keys to CHIP8 keypad */
			case sf::Keyboard::Num1: chip8.keypad[0x1] = false; break;
			case sf::Keyboard::Num2: chip8.keypad[0x2] = false; break;
			case sf::Keyboard::Num3: chip8.keypad[0x3] = false; break;
			case sf::Keyboard::Num4: chip8.keypad[0xC] = false; break;
														 
			case sf::Keyboard::Q:	 chip8.keypad[0x4] = false; break;
			case sf::Keyboard::W:	 chip8.keypad[0x5] = false; break;
			case sf::Keyboard::E:	 chip8.keypad[0x6] = false; break;
			case sf::Keyboard::R:	 chip8.keypad[0xD] = false; break;
														 
			case sf::Keyboard::A:	 chip8.keypad[0x7] = false; break;
			case sf::Keyboard::S:	 chip8.keypad[0x8] = false; break;
			case sf::Keyboard::D:	 chip8.keypad[0x9] = false; break;
			case sf::Keyboard::F:	 chip8.keypad[0xE] = false; break;
														 
			case sf::Keyboard::Z:	 chip8.keypad[0xA] = false; break;
			case sf::Keyboard::X:	 chip8.keypad[0x0] = false; break;
			case sf::Keyboard::C:	 chip8.keypad[0xB] = false; break;
			case sf::Keyboard::V:	 chip8.keypad[0xF] = false; break;

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
			chip8.stack_ptr--);
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
		printf("<0x%X> PC = 0x%X:  PC = 0x%X + V0\n", chip8.inst.opcode, chip8.PC - 2, chip8.inst.NNN);
		break;

	case 0xC:
		printf("<0x%X> PC = 0x%X:  V%X = rand() %% 256 & NN (0x%02X)\n", chip8.inst.opcode, chip8.PC - 2, chip8.regs.V[0], chip8.inst.NN);
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

	case 0xE:
		// EX9E: Skip next instruction if key in VX is pressed
		if (chip8.inst.NN == 0x9E)
		{
			printf("<0x%X> PC = 0x%X:  Skip next instruction if key in V%X (0x%X) is pressed; Keypad value: %d\n",
				chip8.inst.opcode,
				chip8.PC - 2,
				chip8.inst.X,
				chip8.regs.V[chip8.inst.X],
				chip8.keypad[chip8.regs.V[chip8.inst.X]]);
		}

		// EXA1: Skip next instruction if key in VX is NOT pressed
		if (chip8.inst.NN == 0xA1)
		{
			printf("<0x%X> PC = 0x%X:  Skip next instruction if key in V%X (0x%X) is NOT pressed; Keypad value: %d\n",
				chip8.inst.opcode,
				chip8.PC - 2,
				chip8.inst.X,
				chip8.regs.V[chip8.inst.X],
				chip8.keypad[chip8.regs.V[chip8.inst.X]]);
		}
		break;

	case 0xF:
		switch (chip8.inst.NN)
		{
			// FX0A: VX = get_key(); Await until a keypress, and store in VX
		case 0xA:
			printf("<0x%X> PC = 0x%X:  Await until a key is pressed; store key in V%X\n", chip8.inst.opcode, chip8.PC - 2, chip8.inst.X);
			break;

			// FX1E: I += VX; Add Vx to register I. For non-Amiga CHIP8, does not affect VF
		case 0x1E:
			printf("<0x%X> PC = 0x%X:  I += V%X (0x%X += 0x%X)\n", 
				chip8.inst.opcode, 
				chip8.PC - 2, 
				chip8.inst.X, 
				chip8.regs.I, 
				chip8.regs.V[chip8.inst.X]);

			break;
		default:
			break;
		}

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

#ifdef DEBUG_ON
	run_single_opcode_d(chip8, config);
#endif // DEBUG


	uint32_t X_coord;		// Used for DXYN 
	uint32_t Y_coord;		// Used for DXYN
	uint8_t  orig_X;		// Used for DXYN
	bool carry;
	bool any_key_press;

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

	case 0xE:
		// EX9E: Skip next instruction if key in VX is pressed
		if (chip8.inst.NN == 0x9E)
		{
			if (chip8.keypad[chip8.regs.V[chip8.inst.X]])
				chip8.PC += 2;
		}

		// EXA1: Skip next instruction if key in VX is NOT pressed
		if (chip8.inst.NN == 0xA1)
		{
			if (!chip8.keypad[chip8.regs.V[chip8.inst.X]])
				chip8.PC += 2;
		}
		break;

	case 0xF:
		switch (chip8.inst.NN)
		{
		// FX0A: VX = get_key(); Await until a keypress, and store in VX
		case 0xA:
			any_key_press = false;
			for (size_t i = 0; i < sizeof(chip8.keypad); i++)
			{
				if (chip8.keypad[i])
				{
					chip8.regs.V[chip8.inst.X] = i; // i = key (offset of keypad)
					any_key_press = true;
					break;
				}
			}
			// if no keypad has been pressed: Key getting the current opcode and running this instruction
			if (!any_key_press) chip8.PC -= 2; 
			break;

		// FX1E: I += VX; Add Vx to register I. For non-Amiga CHIP8, does not affect VF
		case 0x1E:
			chip8.regs.I += chip8.regs.V[chip8.inst.X];
			break;
		
		// FX07: Vx = delay timer
		case 0x07:
			chip8.regs.V[chip8.inst.X] = chip8.delay_timer;
			break;
			
		// FX15: delay timer = Vx
		case 0x15:
			chip8.delay_timer = chip8.regs.V[chip8.inst.X];
			break;

		// FX18: sound timer = Vx
		case 0x18:
			chip8.sound_timer = chip8.regs.V[chip8.inst.X];
			break;

		default:
			break;
		}
		break;
	
	default:
		break;
	}
}
