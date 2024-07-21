#pragma once



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
		.rom_name = "ROM/3-corax+.ch8"
	};

}

bool init_sfml(sfml_t& sfml, const config_t& config)
{
	uint16_t width = config.screen_width * config.scale_factor;
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