#include <SFML/Window.hpp>

#include "AudioClip.h"
#include "AudioEngine.h"

int main()
{
    AudioEngine engine;

	auto clip = engine.CreateClip("TestAudio/Pew.wav", false);

    sf::Window window = sf::Window(sf::VideoMode(500, 500), "Sounds");

	uint8_t id = 0;

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

        //float deltaTime = clock.restart().asSeconds();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        {
			id = engine.PlaySound(clip, 100, 1, true);
        }

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
		{
			engine.SetAudioState(id, AudioState::Stop);
		}

        engine.Update(0);
    }
}