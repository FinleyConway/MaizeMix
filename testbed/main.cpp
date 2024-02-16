#include <iostream>

#include <SFML/Window.hpp>
#include <entt/entt.hpp>
#include <MaizeMix.h>

#include "test_Components.h"
#include "test_AudioSystem.h"
#include "test_Callback.h"

using namespace Maize;

auto CreateTestDummy(entt::registry& registry, const Mix::AudioClip& clip)
{
	auto entity = registry.create();
	auto& position = registry.emplace<PositionComponent>(entity);
	auto& audio = registry.emplace<AudioSourceComponent>(entity);

	audio.clip = clip;

	return entity;
}

int main()
{
	sf::Window window = sf::Window(sf::VideoMode(500, 500), "Sounds");
	Mix::AudioEngine engine;
	entt::registry registry;

	test_Callback t(registry);

	engine.SetAudioFinishCallback(&t);

	sf::Clock clock;

	test_AudioSystem system;

	auto clip = engine.CreateClip("/home/finley/GameShiz/Sounds/TestMusic.wav", true);

	auto entity = CreateTestDummy(registry, clip);

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
		static bool pressed1 = false;
		static bool pressed2 = false;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
		{
			if (!pressed)
			{
				registry.emplace<PlayAudioSourceTag>(entity);
				pressed = true;
			}
		}
		else
		{
			pressed = false;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
		{
			if (!pressed1)
			{
				registry.emplace<StopAudioSourceTag>(entity);
				pressed1 = true;
			}
		}
		else
		{
			pressed1 = false;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
		{
			if (!pressed2)
			{
				registry.emplace<PauseAudioSourceTag>(entity);
				pressed2 = true;
			}
		}
		else
		{
			pressed2 = false;
		}

		system.Update(deltaTime, registry, engine);
	}
}