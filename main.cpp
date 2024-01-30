#include <SFML/Window.hpp>

#include "AudioClip.h"
#include "AudioEngine.h"

int main()
{
    AudioEngine engine;

    AudioClip soundClip = AudioClip("TestAudio/Pew.wav", false);

    sf::Window window = sf::Window(sf::VideoMode(500, 500), "Sounds");

    bool is = false;
    float currentTime = 0;

    engine.SetListenerPosition(0, 0, 0);

    float x = 0, y = 0;

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

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !is)
        {
            engine.PlaySoundAtPosition(soundClip, 100, 1, true, 0, 0, 0, 5.0f, 10.0f);
			is = true;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) y -= 10 * deltaTime;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) y += 10 * deltaTime;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) x += 10 * deltaTime;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) x -= 10 * deltaTime;

        engine.SetListenerPosition(x, y, 0);

        engine.Update(0);
    }
}