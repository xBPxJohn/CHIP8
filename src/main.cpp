#include <iostream>
#include <CHIP8.hpp>
#include <iomanip> // For std::setw and std::setfill


int main(int argc, char* argv[])
{
#ifdef DEBUG_OFF
	if (argc < 2)
	{
		printf("Usage: PROGAME --rom-name\n-h for help\n");
		return -1;
	}
#endif

	config_t config{ 0 };
	if (!init_config(config, argc, argv))
	{
		return -1;
	}
		

	sfml_t sfml;
	init_sfml(sfml, config);

	chip8_t chip8;
	if (!init_chip8(chip8, config))
	{
		printf("Failed to init chip8\n");
		return -1;
	}
	
	chip8.draw = true;


	
	srand(time(NULL)); // Get seed for random number

	while (sfml.window.isOpen())
	{
		user_input(sfml, chip8);

		sf::sleep(sf::milliseconds(1000 / 60)); // Adjust 60 to desired FPS limit

		run_single_opcode(chip8, config, sfml);

		if (chip8.draw)
		{
			update_screen(sfml, config, chip8);
			chip8.draw = false;
		}

		update_timers(chip8);
	}
	
	
	return 0;
}