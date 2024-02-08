#include "SFML/Window.hpp"
#include <iostream>

#include "MaizeMix/AudioClip.h"
#include "MaizeMix/AudioEngine.h"

using namespace Maize;

int main()
{
	Mix::AudioEngine engine;

	auto sound = engine.CreateClip("TestAudio/Pew.wav", false);
	auto music = engine.CreateClip("/home/finley/GameShiz/Sounds/TestMusic.wav", true);

    sf::Window window = sf::Window(sf::VideoMode(500, 500), "Sounds");

    sf::Clock clock;
	uint8_t id = 0;

	std::cout << sizeof(engine) << std::endl;

    while (window.isOpen())
    {
        sf::Event e;

        while (window.pollEvent(e))
        {
            if (e.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        float deltaTime = clock.restart().asSeconds();

		static bool pressed = false;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
		{
			if (pressed)
			{
				std::cout << (int)engine.PlaySound(sound, 100, 1, false) << std::endl;
				pressed = false;
			}
		}
		else
		{
			pressed = true;
		}

		static bool pressed0 = false;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
		{
			if (pressed0)
			{
				engine.SetAudioState(id, Mix::AudioState::Stop);
				pressed0 = false;
			}
		}
		else
		{
			pressed0 = true;
		}

		static bool pressed1 = false;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
		{
			if (pressed1)
			{
				engine.DestroyClip(sound);
				pressed1 = false;
			}
		}
		else
		{
			pressed1 = true;
		}


		engine.Update(deltaTime);
    }
}