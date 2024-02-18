# MaizeMix

`MaizeMix` is an audio engine designed to streamline audio handling within your application using, using the capabilities of the [SFML](https://github.com/SFML/SFML) audio library module. The design of `MaizeMix` is primarily focused on supporting an Entity-Component-System (ECS) architecture, offering a modular and scalable approach to audio management.

## Code Example

```cpp
#include <MaizeMix/MaizeMix.h>
#include <entt/entt.hpp> // ECS example

struct AudioSourcePlay { };
struct AudioSourcePlaying { uint8_t playingID = 0; };
struct AudioSource
{
	Mix::AudioClip clip;

	bool loop = false;
	bool mute = false;
	float volume = 100;
	float pitch = 1;
};

class FinishCallback : public Mix::AudioFinishCallback
{
 public:
	explicit FinishCallback(entt::registry& registry) : m_Registry(&registry) { }

	void OnAudioFinish(uint8_t audioSourceID, const std::any& userData) override
	{
		auto entity = std::any_cast<entt::entity>(userData);

		m_Registry->remove<AudioSourcePlaying>(entity);
	}

 private:
	entt::registry* m_Registry = nullptr;
};

void Update(float deltaTime, entt::registry& registry, Mix::AudioEngine& audioEngine)
{
	audioEngine.Update(deltaTime);

	auto view = registry.view<AudioSource>();
	for (auto [entity, source] : view.each())
	{
		// play audio source
		if (registry.all_of<AudioSourcePlay>(entity))
		{
			auto userData = entity;
			auto playingID = audioEngine.PlayAudio(source.clip, source.volume, source.pitch, source.loop, userData);

			registry.emplace_or_replace<AudioSourcePlaying>(entity, playingID);
			registry.remove<AudioSourcePlay>(entity);
		}

		// update audio source
		if (registry.all_of<AudioSourcePlaying>(entity))
		{
			auto& playing = registry.get<AudioSourcePlaying>(entity);

			audioEngine.SetAudioLoopState(playing.playingID, source.loop);
			audioEngine.SetAudioMuteState(playing.playingID, source.mute);
			audioEngine.SetAudioPitch(playing.playingID, source.pitch);
			audioEngine.SetAudioVolume(playing.playingID, source.volume);
		}
	}
}

int main()
{
	entt::registry registry;
	Mix::AudioEngine audioEngine;

	// custom callback to handle custom logic for audio that has stopped playing
	auto callback = FinishCallback(registry);
	audioEngine.SetAudioFinishCallback(&callback);

	// create audio clip
	bool shouldClipStream = false;
	auto clip = audioEngine.CreateClip("Kaboom.wav", shouldClipStream);

	// create entity
	auto entity = registry.create();
	auto& source = registry.emplace<AudioSource>(entity);
	source.clip = clip;

	float deltaTime = 0;

	Update(deltaTime, registry, audioEngine);
}
```

## License

MaizeMix is under the [MIT license](https://github.com/FinleyConway/MaizeMix/blob/master/license.md)

## External libraries used by MaizeMix

- [SFML](https://github.com/SFML/SFML) is under the [zLib license](https://github.com/SFML/SFML/blob/master/license.md)
- [entt](https://github.com/skypjack/entt) is under the [MIT license](https://github.com/skypjack/entt/blob/master/LICENSE) (testbed)
