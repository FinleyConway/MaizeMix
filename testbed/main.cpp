#include <SFML/Window.hpp>
#include <entt/entt.hpp>
#include <MaizeMix/MaizeMix.h>
#include <iostream>

#include "test_Components.h"
#include "test_AudioSystem.h"
#include "test_Callback.h"

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

	auto clip = engine.CreateClip("Clips/Pew.wav", false);

	auto entity = CreateTestDummy(registry, clip);
	auto entity2 = CreateTestDummy(registry, clip);

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

        window.setTitle("Sounds - frame time: " + std::to_string(deltaTime));

		static bool pressed = false;
		static bool pressed1 = false;
		static bool pressed2 = false;
		static bool pressed3 = false;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
		{
			if (!pressed)
			{
				registry.emplace_or_replace<PlayAudioSourceTag>(entity);
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
				registry.emplace_or_replace<StopAudioSourceTag>(entity);
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
				registry.emplace_or_replace<PauseAudioSourceTag>(entity);
				pressed2 = true;
			}
		}
		else
		{
			pressed2 = false;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4))
		{
			if (!pressed3)
			{
				registry.emplace_or_replace<UnPauseAudioSourceTag>(entity);
				pressed3 = true;
			}
		}
		else
		{
			pressed3 = false;
		}

		system.Update(deltaTime, registry, engine);
	}
}