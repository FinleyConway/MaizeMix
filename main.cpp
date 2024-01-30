#include <SFML/Window.hpp>

#include "AudioClip.h"
#include "AudioEngine.h"

int main()
{
	AudioEngine engine;

	AudioClip musicClip = AudioClip("/home/finley/GameShiz/Sounds/TestMusic.wav", false);
	AudioClip soundClip = AudioClip("/home/finley/GameShiz/Sounds/Pew.wav", false);

	sf::Window window = sf::Window(sf::VideoMode(500, 500), "Sounds");

	bool is = false;
	float currentTime = 0;

	engine.PlaySound(musicClip, 100, 1, true);

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

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			engine.StopSound(musicClip);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
		{
			engine.PlaySound(musicClip, 100, 1, true);
		}

		engine.Update(0);
	}
}