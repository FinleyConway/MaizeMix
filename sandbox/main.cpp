#include <SFML/Graphics.hpp>
#include <flecs.h>
#include <iostream>
#include <MaizeMix.h>

struct AudioContext
{
	Mix::AudioEngine* engine;
};

struct AudioSource
{
	Mix::AudioClip clip;
	bool loop = false;
	bool mute = false;
	float volume = 100;
	float pitch = 1;

	float time = 0;
};

struct AudioSourcePlay { };
struct AudioSourcePlaying { };
struct AudioSourceStop { };

void OnAudioSourcePlay(flecs::entity entity, AudioSourcePlay)
{
	const auto* context = entity.world().get<AudioContext>();

	if (const auto* source = entity.get_mut<AudioSource>())
	{
		const auto spec = Mix::AudioSpecification(
			source->loop,
			source->volume,
			source->pitch,
			0, 0, 0,
			0, 0
		);

		context->engine->PlayAudio(entity.id(), source->clip, spec);

		entity.add<AudioSourcePlaying>();
	}

	entity.remove<AudioSourcePlay>();
}

void OnAudioSourceStop(flecs::entity entity, AudioSourceStop)
{
	const auto* context = entity.world().get<AudioContext>();

	context->engine->StopAudio(entity);

	entity.remove<AudioSourceStop>();
}

void UpdatePlayingAudio(flecs::entity entity, AudioSource& source, AudioSourcePlaying)
{
	const float deltaTime = entity.world().delta_time();
	const auto* context = entity.world().get<AudioContext>();
	auto* engine = context->engine;

	engine->Update(deltaTime);
	engine->SetAudioLoopState(entity, source.loop);
	engine->SetAudioMuteState(entity, source.mute);
	engine->SetAudioVolume(entity, source.volume);
	engine->SetAudioPitch(entity, source.pitch);
}

int main()
{
	auto window = sf::RenderWindow(sf::VideoMode(500, 500), "Sandbox");

	flecs::world world;
	Mix::AudioEngine engine;

	auto sound = engine.CreateClip("Clips/Pew.wav", false);
	auto stream = engine.CreateClip("Clips/Music.wav", true);

	engine.SetAudioFinishCallback([&](uint64_t entityID)
	{
		auto entity = world.entity(entityID);

		entity.remove<AudioSourcePlaying>();
	});

	world.observer<AudioSourcePlay>().event(flecs::OnAdd).each(OnAudioSourcePlay);
	world.observer<AudioSourceStop>().event(flecs::OnAdd).each(OnAudioSourceStop);
	world.system<AudioSource, const AudioSourcePlaying>().each(UpdatePlayingAudio);

	world.set(AudioContext(&engine));

	auto entity = world.entity()
		.set(AudioSource(stream));

	sf::Clock clock;

	while (window.isOpen())
	{
		sf::Event event;

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
			else if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Space)
				{
					entity.add<AudioSourcePlay>();
				}
				else if (event.key.code == sf::Keyboard::LControl)
				{
					entity.add<AudioSourceStop>();
				}
			}
		}

		world.progress(clock.restart().asSeconds());
	}
}