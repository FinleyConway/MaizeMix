#include <SFML/Graphics.hpp>
#include <flecs.h>
#include <imgui.h>
#include <imgui-SFML.h>
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

struct AudioListener
{
	float volume = 0;
};

struct AudioSourcePlay { };
struct AudioSourcePause { };
struct AudioSourceUnPause { };
struct AudioSourceStop { };
struct AudioSourcePlaying { };

struct AudioClips
{
	Mix::AudioClip sound;
	Mix::AudioClip stream;
};

void OnAudioSourcePlay(flecs::entity entity, AudioSourcePlay)
{
	const auto* context = entity.world().get<AudioContext>();

	if (const auto* source = entity.get<AudioSource>())
	{
		const auto spec = Mix::AudioSpecification(
				source->loop,
				source->mute,
				source->volume,
				source->pitch
			);

		context->engine->PlayAudio(entity, source->clip, spec);

		entity.add<AudioSourcePlaying>();
	}

	entity.remove<AudioSourcePlay>();
}

void OnAudioSourcePause(flecs::entity entity, AudioSourcePause)
{
	const auto* context = entity.world().get<AudioContext>();

	context->engine->PauseAudio(entity);

	entity.remove<AudioSourcePause>();
}

void OnAudioSourceUnPause(flecs::entity entity, AudioSourceUnPause)
{
	const auto* context = entity.world().get<AudioContext>();

	context->engine->UnpauseAudio(entity);

	entity.remove<AudioSourceUnPause>();
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

	engine->SetAudioOffsetTime(entity, source.time);
	source.time = engine->GetAudioOffsetTime(entity);
}

void AudioConfig(flecs::entity entity, AudioSource& source, const AudioClips& clips)
{
	ImGuiStyle& style = ImGui::GetStyle();
	style.FramePadding = { 2, 2 };
	style.ItemSpacing = { 4, 2 };
	style.WindowPadding = { 5, 5 };

	if (ImGui::Begin("Audio manager", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		// display playing status
		if (entity.has<AudioSourcePlaying>())
		{
			ImGui::TextColored({ 255, 0, 0, 255 }, "Audio source playing...");
		}

		// audio control buttons
		if (ImGui::Button("Play audio"))		{ entity.add<AudioSourcePlay>();	} ImGui::SameLine();
		if (ImGui::Button("Pause audio"))		{ entity.add<AudioSourcePause>();	} ImGui::SameLine();
		if (ImGui::Button("Un-pause audio"))	{ entity.add<AudioSourceUnPause>(); } ImGui::SameLine();
		if (ImGui::Button("Stop audio"))		{ entity.add<AudioSourceStop>();	}

		// volume and pitch
		ImGui::SliderFloat("Volume", &source.volume, 0, 100, "%.1f");
		ImGui::SliderFloat("Pitch", &source.pitch, 0.0001f, 100.0f, "%.4f");

		// playback progress
		ImGui::SliderFloat("Progress", &source.time, 0, source.clip.GetDuration(), "%.2f");

		// mute and loop
		ImGui::Checkbox("Mute", &source.mute); ImGui::SameLine();
		ImGui::Checkbox("Loop", &source.loop);

		// need to detect audio clip change
		if (ImGui::Button("Assign Sound")) source.clip = clips.sound; ImGui::SameLine();
		if (ImGui::Button("Assign Stream")) source.clip = clips.stream;
	}
	ImGui::End();
}

int main()
{
	auto window = sf::RenderWindow(sf::VideoMode(1024, 1024), "Sandbox");
	window.setFramerateLimit(60);
	if (!ImGui::SFML::Init(window)) return 1;
	auto view = sf::View({ 0, 0 }, sf::Vector2f(window.getSize()));

	flecs::world world;
	Mix::AudioEngine engine;
	engine.SetAudioFinishCallback([&](uint64_t entityID)
	{
		const auto entity = world.entity(entityID);

		entity.get_mut<AudioSource>()->time = 0;
		entity.remove<AudioSourcePlaying>();
	});

	auto sound = engine.CreateClip("Clips/Pew.wav", false);
	auto stream = engine.CreateClip("Clips/Music.wav", true);

	world.observer<AudioSourcePlay>().event(flecs::OnAdd).each(OnAudioSourcePlay);
	world.observer<AudioSourcePause>().event(flecs::OnAdd).each(OnAudioSourcePause);
	world.observer<AudioSourceUnPause>().event(flecs::OnAdd).each(OnAudioSourceUnPause);
	world.observer<AudioSourceStop>().event(flecs::OnAdd).each(OnAudioSourceStop);

	world.system<AudioSource, const AudioClips>().each(AudioConfig);
	world.system<AudioSource, const AudioSourcePlaying>().each(UpdatePlayingAudio);

	world.set(AudioContext(&engine));

	world.entity().set(AudioListener(100)).add<sf::CircleShape>();
	world.entity().set(AudioSource(sound)).set(AudioClips(sound, stream)).add<sf::CircleShape>();

	sf::Clock clock;

	while (window.isOpen())
	{
		sf::Event event;

		while (window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(window, event);

			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
			else if (event.type == sf::Event::Resized)
			{
				view.setSize(event.size.width, event.size.height);
			}
		}

		float deltaTime = clock.restart().asSeconds();

		ImGui::SFML::Update(window, sf::seconds(deltaTime));

		window.setView(view);

		window.clear();
		world.progress(deltaTime);
		ImGui::SFML::Render(window);
		window.display();
	}
}