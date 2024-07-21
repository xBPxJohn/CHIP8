#pragma once


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
	const uint8_t bg_b = (config.bg_color >> 8) & 0xFF;
	const uint8_t bg_a = (config.bg_color) & 0xFF;

	sfml.window.clear(sf::Color(bg_r, bg_g, bg_b, bg_a));

	const uint8_t fg_r = (config.fg_color >> 24) & 0xFF;
	const uint8_t fg_g = (config.fg_color >> 16) & 0xFF;
	const uint8_t fg_b = (config.fg_color >> 8) & 0xFF;
	const uint8_t fg_a = (config.fg_color) & 0xFF;

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