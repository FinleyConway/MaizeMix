#include <SFML/Window.hpp>

#include "AudioClip.h"
#include "AudioEngine.h"

int main()
{
    AudioEngine engine;

	auto sound = engine.CreateClip("TestAudio/Pew.wav", false);
	auto music = engine.CreateClip("/home/finley/GameShiz/Sounds/TestMusic.wav", true);

    sf::Window window = sf::Window(sf::VideoMode(500, 500), "Sounds");

    sf::Clock clock;

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

		static bool spaceKeyPressed = false;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			if (!spaceKeyPressed)
			{
				engine.PlaySound(sound, 30, 1, false);
				spaceKeyPressed = true;
			}
		}
		else
		{
			spaceKeyPressed = false;
		}

		static bool spaceKeyPressed1 = false;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))
		{
			if (!spaceKeyPressed1)
			{
				engine.PlaySound(music, 30, 1, false);
				spaceKeyPressed1 = true;
			}
		}
		else
		{
			spaceKeyPressed1 = false;
		}

        engine.Update(deltaTime);
    }
}