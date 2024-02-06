#pragma once

#include <SFML/Audio.hpp>

#include <unordered_map>
#include <variant>
#include <memory>
#include <set>
#include <iostream>

class AudioClip;

enum class AudioState
{
	Pause,
	Unpause,
	Mute,
	Unmute,
	Stop
};

class AudioEngine
{
 public:
	AudioEngine()
	{
		m_UnusedIDs.reserve(c_InvalidAudioSource);
		for (uint8_t i = 1; i <= c_MaxAudioEmitters; i++)
		{
			m_UnusedIDs.push_back(i);
		}
	}

	AudioClip CreateClip(const std::string& audioPath, bool stream);
	void DestroyClip(AudioClip& clip);

	uint8_t PlaySound(AudioClip& clip, float volume, float pitch, bool loop);
	uint8_t PlaySoundAtPosition(AudioClip& clip, float volume, float pitch, bool loop, float x, float y, float depth, float minDistance, float maxDistance);

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
		const uint8_t audioSourceID = 0;
		const float stopTime = 0;

		SoundEventData() = default;
		SoundEventData(int16_t clip, float stopTime)
			: stopTime(stopTime), audioSourceID(clip)
		{
		}

		bool operator<(const SoundEventData& other) const
		{
			if (stopTime == other.stopTime)
			{
				return audioSourceID < other.audioSourceID;
			}

			return stopTime < other.stopTime;
		}

	};

    struct Audio
    {
        std::variant<sf::Sound, std::shared_ptr<sf::Music>> sound;
        const SoundEventData* event = nullptr;

        float previousVolume = 0;

        Audio() = default;
        Audio(const std::shared_ptr<sf::Music>& audio, const SoundEventData* event)
			: sound(audio), event(event)
		{
		}
        Audio(const sf::Sound& audio, const SoundEventData* event)
                : sound(audio), event(event)
        {
        }
    };

private:
	float LimitVolume(float volume) const;
	bool HasHitMaxAudioSources() const;
	float GetPlayingOffset(const std::variant<sf::Sound, std::shared_ptr<sf::Music>>& soundVariant);

	void PauseSound(Audio& soundData);
	void UnpauseSound(uint8_t audioSourceID, Audio& soundData);
	void MuteSound(Audio& soundData);
	void UnmuteSound(Audio& soundData);
	void StopSound(uint8_t audioSourceID, Audio& soundData);

	uint8_t PlayAudio(const AudioClip& clip, float volume, float pitch, bool loop, float x = 0.0f, float y = 0.0f, float depth = 0.0f, float minDistance = 5.0f, float maxDistance = 10.0f);
	uint8_t PlayStreamedAudio(AudioClip& clip, float volume, float pitch, bool loop, float x = 0.0f, float y = 0.0f, float depth = 0.0f, float minDistance = 5.0f, float maxDistance = 10.0f);

	uint8_t GetNextID()
	{
		if (m_UnusedIDs.empty()) return c_InvalidAudioSource;

		uint8_t nextID = m_UnusedIDs.back();

		m_UnusedIDs.pop_back();

		return nextID;
	}

	void ReturnID(uint8_t audioSourceID)
	{
		m_UnusedIDs.push_back(audioSourceID);
	}

 private:
	sf::Time m_CurrentTime;

	std::unordered_map<size_t, std::shared_ptr<sf::SoundBuffer>> m_SoundBuffers;
	std::unordered_map<size_t, std::shared_ptr<sf::Music>> m_Music;

	std::unordered_map<uint8_t, Audio> m_CurrentPlayingSounds;
 	std::set<SoundEventData> m_AudioEventQueue;

	std::vector<uint8_t> m_UnusedIDs;

	const uint8_t c_MaxAudioEmitters = 250;
	const size_t c_InvalidClip = 0;
	const uint8_t c_InvalidAudioSource = 0;
};