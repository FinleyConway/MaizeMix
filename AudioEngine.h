#pragma once

#include <SFML/Audio.hpp>

#include <unordered_map>
#include <memory>
#include <set>

class AudioClip;

enum class AudioState
{
	Pause,
	Mute,
	Unmute,
	Stop
};

class AudioEngine
{
 public:
	AudioClip CreateClip(const std::string& audioPath, bool stream);
	void DestroyClip(AudioClip& clip);

	uint8_t PlaySound(const AudioClip& clip, float volume, float pitch, bool loop);
    void PlaySoundAtPosition(const AudioClip& clip, float volume, float pitch, bool loop, float x, float y, float depth, float minDistance, float maxDistance);

	void UpdateSoundLoopState(uint8_t audioSourceID, bool loop);
	void UpdateSoundVolume(uint8_t audioSourceID, float volume);
	void UpdateSoundPitch(uint8_t audioSourceID, float pitch);
	void UpdateSoundPosition(uint8_t audioSourceID, float x, float y, float depth, float minDistance, float maxDistance);

	void SetAudioState(uint8_t audioSourceID, AudioState audioState);

    void SetListenerPosition(float x, float y, float depth);
	void SetGlobalVolume(float volume);

	void Update(float deltaTime);

 private:
	struct SoundEventData
	{
		const int16_t clipID = -1;
		const float stopTime = 0;

		SoundEventData() = default;
		SoundEventData(int16_t clip, float stopTime)
			: stopTime(stopTime), clipID(clip)
		{
		}

		bool operator<(const SoundEventData& other) const
		{
			return stopTime < other.stopTime;
		}
	};

    struct Sound
    {
        sf::Sound sound;
        const SoundEventData* event = nullptr;

        float previousVolume = 0;

        Sound() = default;
        Sound(const sf::Sound& audio, const SoundEventData* event)
                : sound(audio), event(event)
        {
        }
    };

private:
	void PauseSound(Sound& soundData);
	void MuteSound(Sound& soundData);
	void UnmuteSound(Sound& soundData);
	void StopSound(uint8_t audioSourceID, Sound& soundData);

    float LimitVolume(float volume) const
    {
        return std::clamp(volume, 0.0f, 100.0f);;
    }

 public:
 	uint8_t GetCurrentAudioCount() const
	{
		 return m_SoundEventQueue.size();
	}

 private:
	sf::Time m_CurrentTime;

	std::unordered_map<int16_t, std::shared_ptr<sf::SoundBuffer>> m_SoundBuffers;
	std::unordered_map<int16_t, std::shared_ptr<sf::Music>> m_Music;

	std::unordered_map<uint8_t, Sound> m_CurrentPlayingSounds;
 	std::set<SoundEventData> m_SoundEventQueue;

	const uint8_t c_MaxAudioEmitters = 250;
};