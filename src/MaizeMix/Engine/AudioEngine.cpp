#include "MaizeMix/Engine/AudioEngine.h"

#include "MaizeMix/Audio/Data/SoundReference.h"
#include "MaizeMix/Audio/Data/SoundBuffer.h"
#include "MaizeMix/Audio/AudioClip.h"
#include "MaizeMix/Audio/Music.h"

#include <iostream>
#include <cmath>
#include <cassert>

namespace Mix {

	uint8_t AudioEngine::PlayAudio(const AudioClip& clip, const AudioSpecification& spec, uint64_t entity)
	{
		if (HasHitMaxAudioSources()) return c_InvalidAudioSource;

		if (const auto handle = clip.m_Handle.lock())
		{
			const uint8_t audioSourceID = GetNextID();

			if (clip.IsLoadInBackground())
			{
			}
			else
			{
				m_SoundHandler.PlayClip(static_cast<SoundBuffer&>(*handle), spec, entity, m_AudioEventQueue, audioSourceID, m_CurrentTime.asSeconds());
			}

			return audioSourceID;
		}

		return c_InvalidAudioSource;
	}

	void AudioEngine::PauseAudio(uint8_t playingID)
	{
		if (m_SoundHandler.HasEmitter(playingID))
		{
			m_SoundHandler.PauseClip(playingID, m_AudioEventQueue);
		}
	}

	void AudioEngine::UnpauseAudio(uint8_t playingID)
	{
		if (m_SoundHandler.HasEmitter(playingID))
		{
			m_SoundHandler.UnPauseClip(playingID, m_AudioEventQueue, m_CurrentTime.asSeconds());
		}
	}

	void AudioEngine::StopAudio(uint8_t playingID)
	{
		if (m_SoundHandler.HasEmitter(playingID))
		{
			m_SoundHandler.StopClip(playingID, m_AudioEventQueue, m_OnAudioFinish);
		}

		ReturnID(playingID);
	}

	void AudioEngine::SetAudioLoopState(uint8_t playingID, bool loop)
	{
		if (m_SoundHandler.HasEmitter(playingID))
		{
			m_SoundHandler.SetLoopState(playingID, m_AudioEventQueue, m_CurrentTime.asSeconds(), loop);
		}
	}

	void AudioEngine::SetAudioMuteState(uint8_t playingID, bool mute)
	{
		if (m_SoundHandler.HasEmitter(playingID))
		{
			m_SoundHandler.SetMuteState(playingID, mute);
		}
	}

	void AudioEngine::SetAudioVolume(uint8_t playingID, float volume)
	{
		if (m_SoundHandler.HasEmitter(playingID))
		{
			m_SoundHandler.SetVolume(playingID, volume);
		}
	}

	void AudioEngine::SetAudioPitch(uint8_t playingID, float pitch)
	{
		if (m_SoundHandler.HasEmitter(playingID))
		{
			m_SoundHandler.SetPitch(playingID, pitch);
		}
	}

	void AudioEngine::SetAudioPosition(uint8_t playingID, float x, float y, float depth, float minDistance, float maxDistance)
	{
		if (m_SoundHandler.HasEmitter(playingID))
		{
			m_SoundHandler.SetPosition(playingID, x, y, depth, minDistance, maxDistance);
		}
	}

	void AudioEngine::SetSpatialMode(uint8_t playingID, bool isSpatial)
	{
		if (m_SoundHandler.HasEmitter(playingID))
		{
			m_SoundHandler.SetSpatialState(playingID, isSpatial);
		}
	}

    void AudioEngine::SetAudioOffsetTime(uint8_t playingID, float time)
    {
		if (m_SoundHandler.HasEmitter(playingID))
		{
			m_SoundHandler.SetAudioOffsetTime(playingID, m_AudioEventQueue, m_CurrentTime.asSeconds(), time);
		}
    }

    float AudioEngine::GetAudioOffsetTime(uint8_t playingID)
    {
		if (m_SoundHandler.HasEmitter(playingID))
		{
			return m_SoundHandler.GetAudioOffsetTime(playingID);
		}

        return 0.0f;
    }

	void AudioEngine::SetListenerPosition(float x, float y, float depth) const
	{
        // causes backend issues if this isn't here, mainly because audio doesn't exist to offset other emitters
		if (m_SoundHandler.HasEmitters())
		{
			sf::Listener::setPosition(x, y, depth);
		}
	}

	void AudioEngine::SetGlobalVolume(float volume) const
	{
        // causes backend issues if this isn't here, mainly because audio doesn't exist to offset other emitters
		if (m_SoundHandler.HasEmitters())
		{
			sf::Listener::setGlobalVolume(std::clamp(volume, 0.0f, 100.0f));
		}
	}

	void AudioEngine::SetAudioFinishCallback(std::function<void(uint8_t, uint64_t)>&& callback)
	{
		m_OnAudioFinish = callback;
	}

	void AudioEngine::Update(float deltaTime)
	{
		// update audio system time
		m_CurrentTime += sf::seconds(deltaTime);

		// remove all finished sounds
		while (!m_AudioEventQueue.empty() && m_CurrentTime.asSeconds() >= m_AudioEventQueue.begin()->stopTime)
		{
			const auto audioSourceID = m_AudioEventQueue.begin()->playingID;

			if (m_SoundHandler.HasEmitter(audioSourceID))
				m_OnAudioFinish(audioSourceID, m_SoundHandler.GetEmitter(audioSourceID).entity);

			m_SoundHandler.RemoveEmitter(audioSourceID);
			m_AudioEventQueue.erase(m_AudioEventQueue.begin());

			ReturnID(audioSourceID);
		}
	}

	bool AudioEngine::HasHitMaxAudioSources() const
	{
		if (m_AudioEventQueue.size() >= c_MaxAudioEmitters)
		{
			std::cerr << "Warning: Max audio reached! Cannot play more audio clips!" << std::endl;
			return true;
		}

		return false;
	}

	uint8_t AudioEngine::GetNextID()
	{
		static uint8_t newID = 1;

		// generate a new ID if there is non have been returned
		if (m_UnusedIDs.empty())
		{
			// prevent generating an ID greater than the max audio limitation
			if (newID > c_MaxAudioEmitters) return c_InvalidAudioSource;

			return newID++;
		}

		// recycle an ID which was returned
		const uint8_t nextID = m_UnusedIDs.back();
		m_UnusedIDs.pop_back();

		return nextID;
	}

	void AudioEngine::ReturnID(uint8_t audioSourceID)
	{
		m_UnusedIDs.emplace_back(audioSourceID);
	}

} // Mix