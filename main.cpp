#include <SFML/Window.hpp>

#include "AudioClip.h"
#include "AudioEngine.h"

int main()
{
	AudioEngine engine;

	AudioClip soundClip = AudioClip("/home/finley/Desktop/pew.wav", false);

	sf::Window window = sf::Window(sf::VideoMode(500, 500), "Sounds");

	bool is = false;
	float currentTime = 0;

	engine.PlaySound(soundClip, 100, 1, true);

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
            if (is)
            {
                engine.UnMuteSound(soundClip);
                is = false;
            }
            else
            {
                engine.MuteSound(soundClip);
                is = true;
            }
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
		{
			engine.PlaySound(soundClip, 100, 1, true);
		}

		engine.Update(0);

        std::cout << (int)engine.GetCurrentAudioCount() << std::endl;
	}
}