#include <iostream>
#include <CHIP8.hpp>
#include <iomanip> // For std::setw and std::setfill


// Function to print memory contents as hexadecimal
void printMemoryHex(const void* address, size_t nbytes) {
	const unsigned char* bytes = static_cast<const unsigned char*>(address);
	constexpr size_t bytesPerLine = 16; // Number of bytes per line

	for (size_t i = 0; i < nbytes; i += bytesPerLine) {
		// Print address
		std::cout << "0x" << std::setw(8) << std::setfill('0') << std::hex << reinterpret_cast<uintptr_t>(address) + i << ": ";

		// Print hex values for the current line
		for (size_t j = 0; j < bytesPerLine; ++j) {
			if (i + j < nbytes) {
				std::cout << std::setw(2) << std::setfill('0') << static_cast<unsigned>(bytes[i + j]) << " ";
			}
			else {
				std::cout << "   "; // Space for formatting when less than full line
			}
		}

		// Print ASCII representation
		std::cout << "   ";
		for (size_t j = 0; j < bytesPerLine && i + j < nbytes; ++j) {
			unsigned char c = bytes[i + j];
			if (c >= 0x20 && c <= 0x7E) {
				std::cout << c; // Printable character
			}
			else {
				std::cout << "."; // Non-printable character
			}
		}

		std::cout << std::endl;
	}
}

int main(int argc, char* argv[])
{
	config_t config{ 0 };
	init_config(config, argc, argv);

	sfml_t sfml;
	init_sfml(sfml, config);

	chip8_t chip8;
	if (!init_chip8(chip8, config))
	{
		printf("Failed to init chip8\n");
		return -1;
	}

	//printMemoryHex(chip8.ram + 512, 4096);

	chip8.draw = true;
	

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
	}
	

	return 0;
}