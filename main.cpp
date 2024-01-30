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

    AudioEngine::SetListenerPosition(0, 0, 0);

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
            engine.PlaySoundAtPosition(soundClip, 100, 1, true, 0, 0, 0, 5.0f, 10.0f);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) y -= 100 * deltaTime;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) y += 100 * deltaTime;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) x += 100 * deltaTime;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) x -= 100 * deltaTime;

        AudioEngine::SetListenerPosition(x, y, 0);

        engine.Update(0);
    }
}