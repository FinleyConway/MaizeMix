#include <iostream>

#include <SFML/Window.hpp>
#include <entt/entt.hpp>
#include <MaizeMix.h>

#include "test_Components.h"
#include "test_AudioSystem.h"

using namespace Maize;

uint8_t CreateTestDummy(entt::registry& registry, Mix::AudioEngine& engine)
{
	auto entity = registry.create();
	auto& position = registry.emplace<PositionComponent>(entity);
	auto& audio = registry.emplace<AudioSourceComponent>(entity);

	audio.clip = engine.CreateClip("/home/finley/GameShiz/Sounds/Pew.wav", false);
}

int main()
{
	sf::Window window = sf::Window(sf::VideoMode(500, 500), "Sounds");
	Mix::AudioEngine engine;
	entt::registry registry;

	sf::Clock clock;

	test_AudioSystem system;

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

		system.Update(deltaTime, registry);
	}
}