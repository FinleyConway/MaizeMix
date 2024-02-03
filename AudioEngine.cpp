#include "AudioEngine.h"
#include "AudioClip.h"

#include <iostream>
#include <cmath>
#include <cassert>

AudioClip AudioEngine::CreateClip(const std::string& audioPath, bool stream)
{
	// sfml boilerplate to create audio data
	if (stream)
	{
		auto music = std::make_shared<sf::Music>();

		if (music->openFromFile(audioPath))
		{
			const auto id = 0; // TODO: ID Generation

			m_Music.emplace(id, music);

			return {id,music->getChannelCount(),
					music->getDuration().asSeconds(),
					music->getSampleRate(), stream, AudioClip::LoadState::Loaded};
		}
	}
	else
	{
		auto buffer = std::make_shared<sf::SoundBuffer>();

		if (buffer->loadFromFile(audioPath))
		{
			const auto id = 0; // TODO: ID Generation

			m_SoundBuffers.emplace(id, buffer);

			return {id,buffer->getChannelCount(),
					buffer->getDuration().asSeconds(),
					buffer->getSampleRate(), stream, AudioClip::LoadState::Loaded};
		}
	}

	// return a failed audio clip
	return { -1, stream, AudioClip::LoadState::Failed };
}

void AudioEngine::DestroyClip(AudioClip& clip)
{
	if (clip.m_IsStreaming)
	{
		m_Music.erase(clip.m_ClipID);
	}
	else
	{
		m_SoundBuffers.erase(clip.m_ClipID);
	}

	clip = AudioClip();
}

uint8_t AudioEngine::PlaySound(const AudioClip& clip, float volume, float pitch, bool loop)
{
	if (clip.m_ClipID == -1) return 0;

    if (GetCurrentAudioCount() >= c_MaxAudioEmitters)
    {
        std::cerr << "Warning: Max audio reached! Cannot play more audio clips!" << std::endl;
		return 0;
    }

	const auto& buffer = m_SoundBuffers.at(clip.m_ClipID);

	if (buffer != nullptr)
	{
		sf::Sound sound;
		sound.setBuffer(*buffer);
		sound.setVolume(LimitVolume(volume));
		sound.setPitch(pitch);
		sound.setLoop(loop);

		float stopTime;

		if (loop)
		{
			stopTime = std::numeric_limits<float>::infinity();
		}
		else
		{
			stopTime = clip.GetDuration();
		}

		const auto& event = m_SoundEventQueue.emplace(clip.m_ClipID, stopTime);

		// starts at one, zero is the invalid
		const uint8_t audioSourceID = 1; // TODO: ID Generation

		m_CurrentPlayingSounds.try_emplace(audioSourceID, sound, &(*event.first));

		m_CurrentPlayingSounds[audioSourceID].sound.play();
		return audioSourceID;
	}

	return 0;
}

void AudioEngine::PlaySoundAtPosition(const AudioClip &clip, float volume, float pitch, bool loop, float x, float y, float depth, float minDistance, float maxDistance)
{
	/*
	if (clip.m_ClipID == -1) return;

    if (GetCurrentAudioCount() >= c_MaxAudioEmitters)
    {
        std::cerr << "Warning: Max audio reached!" << std::endl;
    }

    const auto& buffer = AudioRequester::GetClipData(clip);

    if (buffer != nullptr)
    {
        sf::Sound sound;
        sound.setBuffer(*buffer);
        sound.setVolume(LimitVolume(volume));
        sound.setPitch(pitch);
        sound.setLoop(loop);
        sound.setPosition(x, y, depth);
        sound.setMinDistance(minDistance);
        sound.setAttenuation(maxDistance);

        float stopTime;

        if (loop)
        {
            stopTime = std::numeric_limits<float>::infinity();
        }
        else
        {
            stopTime = clip.GetDuration();
        }

        const auto& event = m_SoundEventQueue.emplace(clip.m_ClipID, stopTime);
        m_CurrentPlayingSounds.try_emplace(clip.m_ClipID, sound, buffer, &(*event.first));

        m_CurrentPlayingSounds[clip.m_ClipID].sound.play();
    }
    */
}

void AudioEngine::UpdateSoundLoopState(uint8_t audioSourceID, bool loop)
{
	if (m_CurrentPlayingSounds.contains(audioSourceID))
	{
		auto& soundData = m_CurrentPlayingSounds.at(audioSourceID);

		// update sound loop state
		soundData.sound.setLoop(loop);

		// remove current sound event
		assert(soundData.event != nullptr);
		m_SoundEventQueue.erase(*soundData.event);

		// recreate sound event with new event time
		float stopTime = 0;

		if (loop)
		{
			stopTime = std::numeric_limits<float>::infinity();
		}
		else
		{
			// figure out if i should set the length or current playing offset
			stopTime = soundData.sound.getPlayingOffset().asSeconds(); // needs testing
		}

		m_SoundEventQueue.emplace(audioSourceID, stopTime);
	}
}

void AudioEngine::UpdateSoundVolume(uint8_t audioSourceID, float volume)
{
	if (m_CurrentPlayingSounds.contains(audioSourceID))
	{
		auto& soundData = m_CurrentPlayingSounds.at(audioSourceID);

		soundData.sound.setVolume(LimitVolume(volume));
	}
}

void AudioEngine::UpdateSoundPitch(uint8_t audioSourceID, float pitch)
{
	if (m_CurrentPlayingSounds.contains(audioSourceID))
	{
		auto& soundData = m_CurrentPlayingSounds.at(audioSourceID);

		soundData.sound.setPitch(pitch);
	}
}

void AudioEngine::UpdateSoundPosition(uint8_t audioSourceID, float x, float y, float depth, float minDistance, float maxDistance)
{
	if (m_CurrentPlayingSounds.contains(audioSourceID))
	{
		auto& soundData = m_CurrentPlayingSounds.at(audioSourceID);

		soundData.sound.setPosition(x, y, depth);
		soundData.sound.setMinDistance(minDistance);
		soundData.sound.setAttenuation(maxDistance);
	}
}

void AudioEngine::SetAudioState(uint8_t audioSourceID, AudioState audioState)
{
	if (!m_CurrentPlayingSounds.contains(audioSourceID)) return;

	auto& soundData = m_CurrentPlayingSounds.at(audioSourceID);

	if (audioState == AudioState::Pause)
	{
		PauseSound(soundData);
	}
	else if (audioState == AudioState::Mute)
	{
		MuteSound(soundData);
	}
	else if (audioState == AudioState::Unmute)
	{
		UnmuteSound(soundData);
	}
	else if (audioState == AudioState::Stop)
	{
		StopSound(audioSourceID, soundData);
	}
}

void AudioEngine::Update(float currentTime)
{
	// remove all finished sounds
	while (!m_SoundEventQueue.empty() && currentTime >= m_SoundEventQueue.begin()->stopTime)
	{
		m_SoundEventQueue.erase(m_SoundEventQueue.begin());
		m_CurrentPlayingSounds.erase(m_SoundEventQueue.begin()->clipID);
	}
}

void AudioEngine::SetListenerPosition(float x, float y, float depth)
{
	if (m_CurrentPlayingSounds.empty()) return;

	sf::Listener::setPosition(x, y, depth);
}

void AudioEngine::SetGlobalVolume(float volume)
{
	if (m_CurrentPlayingSounds.empty()) return;

	sf::Listener::setGlobalVolume(LimitVolume(volume));
}

void AudioEngine::PauseSound(Sound& soundData)
{
	soundData.sound.pause();
	m_SoundEventQueue.erase(*soundData.event);
}

void AudioEngine::MuteSound(Sound& soundData)
{
	soundData.previousVolume = soundData.sound.getVolume();
	soundData.sound.setVolume(0);
}

void AudioEngine::UnmuteSound(Sound& soundData)
{
	soundData.sound.setVolume(soundData.previousVolume);
}

void AudioEngine::StopSound(uint8_t audioSourceID, Sound& soundData)
{
	soundData.sound.stop();

	assert(soundData.event != nullptr);

	m_SoundEventQueue.erase(*soundData.event);
	m_CurrentPlayingSounds.erase(audioSourceID);
}