#pragma once


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
	uint8_t V[16]; // V0-VF registers

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
	bool keypad[16];
	uint8_t delay_timer;
	uint8_t sound_timer;
	bool draw;
};