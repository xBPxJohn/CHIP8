#pragma once


#include <stdint.h>
#include <memory>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "structs.hpp"
#include "init.hpp"
#include "user_interface.hpp"


void update_timers(chip8_t& chip8)
{
	if (chip8.delay_timer > 0)
		chip8.delay_timer--;

	if (chip8.sound_timer > 0)
		chip8.sound_timer--;

}

#ifdef DEBUG_ON
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
#endif


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

	// 3XNN: if Vx == NN
	case 0x3:
		if (chip8.regs.V[chip8.inst.X] == chip8.inst.NN)
			chip8.PC += 2;
		break;

	// 4XNN: if Vx != NN
	case 0x4:
		if (chip8.regs.V[chip8.inst.X] != chip8.inst.NN)
			chip8.PC += 2;
		break;

	// 5XNN: if Vx == Vy
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


	// 9XNN: if Vx != Vy
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

			for (auto& key : chip8.keypad)
			{
				if (chip8.keypad[key])
				{
					chip8.regs.V[chip8.inst.X] = key; // key (offset of keypad)
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
