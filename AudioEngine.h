#pragma once

#include <SFML/Audio.hpp>

#include <unordered_map>
#include <cstdint>
#include <cmath>
#include <iostream>
#include <memory>

class AudioClip;

class AudioEngine
{
 public:
	void PlaySound(const AudioClip& clip, float volume, float pitch, bool loop);
    void PlaySoundAtPosition(const AudioClip& clip, float volume, float pitch, bool loop, float x, float y, float depth, float minDistance, float maxDistance);
    void PauseSound(const AudioClip& clip);
    void MuteSound(const AudioClip& clip);
    void UnMuteSound(const AudioClip& clip);
	void StopSound(const AudioClip& clip);

    void SetListenerPosition(float x, float y, float depth)
    {
		if (m_CurrentPlayingSounds.empty()) return;

        sf::Listener::setPosition(x, y, depth);
    }

	void SetGlobalVolume(float volume)
    {
		if (m_CurrentPlayingSounds.empty()) return;

        sf::Listener::setGlobalVolume(volume);
    }

	void Update(float currentTime);

 private:
	struct SoundEventData
	{
		int16_t clipID = -1;
		float stopTime = 0;

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
        std::shared_ptr<const sf::SoundBuffer> buffer = nullptr;
        const SoundEventData* event = nullptr;

        float previousVolume = 0;

        Sound() = default;
        Sound(const sf::Sound& audio, const std::shared_ptr<const sf::SoundBuffer>& buffer, const SoundEventData* event)
                : sound(audio), buffer(buffer), event(event)
        {
        }
    };

private:
    float LimitVolume(float volume) const
    {
        return std::clamp(volume, 0.0f, 100.0f);;
    }

 	uint8_t GetCurrentAudioCount() const
	{
		 return m_SoundEventQueue.size();
	}

 private:
	std::unordered_map<int16_t, Sound> m_CurrentPlayingSounds;
 	std::set<SoundEventData> m_SoundEventQueue;

	const uint8_t c_MaxAudioEmitters = 250;
};