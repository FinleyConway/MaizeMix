#include "MaizeMix/AudioEngine.h"

#include "MaizeMix/Streaming/SoundReference.h"
#include "MaizeMix/Sounds/SoundBuffer.h"
#include "MaizeMix/AudioClip.h"

#include <iostream>
#include <cmath>
#include <cassert>

namespace Mix {

	AudioClip AudioEngine::CreateClip(const std::string& filePath, bool stream)
	{
		return m_AudioManager.CreateClip(filePath, stream);
	}

	void AudioEngine::RemoveClip(AudioClip& clip)
	{
		m_AudioManager.DestroyClip(clip);
	}

	bool AudioEngine::PlayAudio(uint64_t entityID, const AudioClip& clip, const AudioSpecification& spec)
	{
		if (HasHitMaxAudioSources()) return false;

		if (const auto handle = clip.m_Handle.lock())
		{
			// stop if the entity is current playing
			StopAudio(entityID);

			if (clip.IsLoadInBackground())
			{
				return m_StreamHandler.PlayClip(entityID, static_cast<SoundReference&>(*handle), spec, m_AudioEventQueue, m_CurrentTime.asSeconds());
			}

			return m_SoundHandler.PlayClip(entityID, static_cast<SoundBuffer&>(*handle), spec, m_AudioEventQueue, m_CurrentTime.asSeconds());
		}

		return false;
	}

	void AudioEngine::PauseAudio(uint64_t entityID)
	{
		if (m_SoundHandler.HasEmitter(entityID))
		{
			m_SoundHandler.PauseClip(entityID, m_AudioEventQueue);
		}
		else if (m_StreamHandler.HasEmitter(entityID))
		{
			m_StreamHandler.PauseClip(entityID, m_AudioEventQueue);
		}
	}

	void AudioEngine::UnpauseAudio(uint64_t entityID)
	{
		if (m_SoundHandler.HasEmitter(entityID))
		{
			m_SoundHandler.UnPauseClip(entityID, m_AudioEventQueue, m_CurrentTime.asSeconds());
		}
		else if (m_StreamHandler.HasEmitter(entityID))
		{
			m_StreamHandler.UnPauseClip(entityID, m_AudioEventQueue, m_CurrentTime.asSeconds());
		}
	}

	void AudioEngine::StopAudio(uint64_t entityID)
	{
		if (m_SoundHandler.HasEmitter(entityID))
		{
			m_SoundHandler.StopClip(entityID, m_AudioEventQueue, m_OnAudioFinish);
		}
		else if (m_StreamHandler.HasEmitter(entityID))
		{
			m_StreamHandler.StopClip(entityID, m_AudioEventQueue, m_OnAudioFinish);
		}
	}

	void AudioEngine::SetAudioLoopState(uint64_t entityID, bool loop)
	{
		if (m_SoundHandler.HasEmitter(entityID))
		{
			m_SoundHandler.SetLoopState(entityID, m_AudioEventQueue, m_CurrentTime.asSeconds(), loop);
		}
		else if (m_StreamHandler.HasEmitter(entityID))
		{
			m_StreamHandler.SetLoopState(entityID, m_AudioEventQueue, m_CurrentTime.asSeconds(), loop);
		}
	}

	void AudioEngine::SetAudioMuteState(uint64_t entityID, bool mute)
	{
		if (m_SoundHandler.HasEmitter(entityID))
		{
			m_SoundHandler.SetMuteState(entityID, mute);
		}
		else if (m_StreamHandler.HasEmitter(entityID))
		{
			m_StreamHandler.SetMuteState(entityID, mute);
		}
	}

	void AudioEngine::SetAudioVolume(uint64_t entityID, float volume)
	{
		if (m_SoundHandler.HasEmitter(entityID))
		{
			m_SoundHandler.SetVolume(entityID, volume);
		}
		else if (m_StreamHandler.HasEmitter(entityID))
		{
			m_StreamHandler.SetVolume(entityID, volume);
		}
	}

	void AudioEngine::SetAudioPitch(uint64_t entityID, float pitch)
	{
		if (m_SoundHandler.HasEmitter(entityID))
		{
			m_SoundHandler.SetPitch(entityID, pitch);
		}
		else if (m_StreamHandler.HasEmitter(entityID))
		{
			m_StreamHandler.SetPitch(entityID, pitch);
		}
	}

	void AudioEngine::SetAudioPosition(uint64_t entityID, float x, float y, float depth, float minDistance, float maxDistance)
	{
		if (m_SoundHandler.HasEmitter(entityID))
		{
			m_SoundHandler.SetPosition(entityID, x, y, depth, minDistance, maxDistance);
		}
		else if (m_StreamHandler.HasEmitter(entityID))
		{
			m_StreamHandler.SetPosition(entityID, x, y, depth, minDistance, maxDistance);
		}
	}

	void AudioEngine::SetSpatialMode(uint64_t entityID, bool isSpatial)
	{
		if (m_SoundHandler.HasEmitter(entityID))
		{
			m_SoundHandler.SetSpatialState(entityID, isSpatial);
		}
		else if (m_StreamHandler.HasEmitter(entityID))
		{
			m_StreamHandler.SetSpatialState(entityID, isSpatial);
		}
	}

    void AudioEngine::SetAudioOffsetTime(uint64_t entityID, float time)
    {
		if (m_SoundHandler.HasEmitter(entityID))
		{
			m_SoundHandler.SetAudioOffsetTime(entityID, m_AudioEventQueue, m_CurrentTime.asSeconds(), time);
		}
		else if (m_StreamHandler.HasEmitter(entityID))
		{
			m_StreamHandler.SetAudioOffsetTime(entityID, m_AudioEventQueue, m_CurrentTime.asSeconds(), time);
		}
    }

    float AudioEngine::GetAudioOffsetTime(uint64_t entityID)
    {
		if (m_SoundHandler.HasEmitter(entityID))
		{
			return m_SoundHandler.GetAudioOffsetTime(entityID);
		}
		else if (m_StreamHandler.HasEmitter(entityID))
		{
			return m_StreamHandler.GetAudioOffsetTime(entityID);
		}

        return 0.0f;
    }

	void AudioEngine::SetListenerPosition(float x, float y, float depth) const
	{
        // causes backend issues if this isn't here, mainly because audio doesn't exist to offset other emitters
		if (m_SoundHandler.HasEmitters() || m_StreamHandler.HasEmitters())
		{
			sf::Listener::setPosition(x, y, depth);
		}
	}

	void AudioEngine::SetGlobalVolume(float volume) const
	{
        // causes backend issues if this isn't here, mainly because audio doesn't exist to offset other emitters
		if (m_SoundHandler.HasEmitters() || m_StreamHandler.HasEmitters())
		{
			sf::Listener::setGlobalVolume(std::clamp(volume, 0.0f, 100.0f));
		}
	}

	void AudioEngine::SetAudioFinishCallback(std::function<void(uint64_t)>&& callback)
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
			const auto audioSourceID = m_AudioEventQueue.begin()->entityID;

			if (m_SoundHandler.HasEmitter(audioSourceID))
			{
				m_OnAudioFinish(m_SoundHandler.GetEmitter(audioSourceID).entity);
				m_SoundHandler.RemoveEmitter(audioSourceID);
			}
			else if (m_StreamHandler.HasEmitter(audioSourceID))
			{
				m_OnAudioFinish(m_StreamHandler.GetEmitter(audioSourceID).entity);
				m_StreamHandler.RemoveEmitter(audioSourceID);
			}

			m_AudioEventQueue.erase(m_AudioEventQueue.begin());
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

} // Mix