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

	bool AudioEngine::PauseAudio(uint64_t entityID)
	{
		if (m_SoundHandler.HasEmitter(entityID))
		{
			return m_SoundHandler.PauseClip(entityID, m_AudioEventQueue);
		}

		if (m_StreamHandler.HasEmitter(entityID))
		{
			return m_StreamHandler.PauseClip(entityID, m_AudioEventQueue);
		}

		return false;
	}

	bool AudioEngine::UnpauseAudio(uint64_t entityID)
	{
		if (m_SoundHandler.HasEmitter(entityID))
		{
			return m_SoundHandler.UnPauseClip(entityID, m_AudioEventQueue, m_CurrentTime.asSeconds());
		}

		if (m_StreamHandler.HasEmitter(entityID))
		{
			return m_StreamHandler.UnPauseClip(entityID, m_AudioEventQueue, m_CurrentTime.asSeconds());
		}

		return false;
	}

	bool AudioEngine::StopAudio(uint64_t entityID)
	{
		if (m_SoundHandler.HasEmitter(entityID))
		{
			m_SoundHandler.StopClip(entityID, m_AudioEventQueue, m_OnAudioFinish);

			return true;
		}

		if (m_StreamHandler.HasEmitter(entityID))
		{
			m_StreamHandler.StopClip(entityID, m_AudioEventQueue, m_OnAudioFinish);

			return true;
		}

		return false;
	}

	bool AudioEngine::SetAudioLoopState(uint64_t entityID, bool loop)
	{
		if (m_SoundHandler.HasEmitter(entityID))
		{
			return m_SoundHandler.SetLoopState(entityID, m_AudioEventQueue, m_CurrentTime.asSeconds(), loop);
		}

		if (m_StreamHandler.HasEmitter(entityID))
		{
			return m_StreamHandler.SetLoopState(entityID, m_AudioEventQueue, m_CurrentTime.asSeconds(), loop);
		}

		return false;
	}

	bool AudioEngine::SetAudioMuteState(uint64_t entityID, bool mute)
	{
		if (m_SoundHandler.HasEmitter(entityID))
		{
			return m_SoundHandler.SetMuteState(entityID, mute);
		}

		if (m_StreamHandler.HasEmitter(entityID))
		{
			return m_StreamHandler.SetMuteState(entityID, mute);
		}

		return false;
	}

	bool AudioEngine::SetAudioVolume(uint64_t entityID, float volume)
	{
		if (m_SoundHandler.HasEmitter(entityID))
		{
			return m_SoundHandler.SetVolume(entityID, volume);
		}

		if (m_StreamHandler.HasEmitter(entityID))
		{
			return m_StreamHandler.SetVolume(entityID, volume);
		}

		return false;
	}

	bool AudioEngine::SetAudioPitch(uint64_t entityID, float pitch)
	{
		if (m_SoundHandler.HasEmitter(entityID))
		{
			return m_SoundHandler.SetPitch(entityID, pitch);
		}

		if (m_StreamHandler.HasEmitter(entityID))
		{
			return m_StreamHandler.SetPitch(entityID, pitch);
		}

		return false;
	}

    bool AudioEngine::SetAudioOffsetTime(uint64_t entityID, float time)
    {
		if (m_SoundHandler.HasEmitter(entityID))
		{
			return m_SoundHandler.SetAudioOffsetTime(entityID, m_AudioEventQueue, m_CurrentTime.asSeconds(), time);
		}

		if (m_StreamHandler.HasEmitter(entityID))
		{
			return m_StreamHandler.SetAudioOffsetTime(entityID, m_AudioEventQueue, m_CurrentTime.asSeconds(), time);
		}

		return false;
    }

    float AudioEngine::GetAudioOffsetTime(uint64_t entityID)
    {
		if (m_SoundHandler.HasEmitter(entityID))
		{
			return m_SoundHandler.GetAudioOffsetTime(entityID);
		}

		if (m_StreamHandler.HasEmitter(entityID))
		{
			return m_StreamHandler.GetAudioOffsetTime(entityID);
		}

        return 0.0f;
    }

	bool AudioEngine::SetListenerPosition(float x, float y, float depth) const
	{
        // causes backend issues if this isn't here, mainly because audio doesn't exist to offset other emitters
		if (m_SoundHandler.HasEmitters() || m_StreamHandler.HasEmitters())
		{
			sf::Listener::setPosition(x, y, depth);

			return true;
		}

		return false;
	}

	bool AudioEngine::SetGlobalVolume(float volume) const
	{
        // causes backend issues if this isn't here, mainly because audio doesn't exist to offset other emitters
		if (m_SoundHandler.HasEmitters() || m_StreamHandler.HasEmitters())
		{
			sf::Listener::setGlobalVolume(std::clamp(volume, 0.0f, 100.0f));

			return true;
		}

		return false;
	}

	void AudioEngine::SetAudioFinishCallback(std::function<void(uint64_t)>&& callback)
	{
		m_OnAudioFinish = callback;
	}

	bool AudioEngine::HasHitMaxAudioSources() const
	{
		if (m_AudioEventQueue.size() >= c_MaxAudioEmitters)
		{
			return true;
		}

		return false;
	}

	uint8_t AudioEngine::EmitterCount() const
	{
		return m_AudioEventQueue.size();
	}

	void AudioEngine::Update(float deltaTime)
	{
		// update audio system time
		m_CurrentTime += sf::seconds(deltaTime);

		// remove all finished sounds
		while (!m_AudioEventQueue.empty() && m_CurrentTime.asSeconds() >= m_AudioEventQueue.begin()->stopTime)
		{
			const auto entityID = m_AudioEventQueue.begin()->entityID;

			if (m_SoundHandler.HasEmitter(entityID))
			{
				m_SoundHandler.RemoveEmitter(entityID);
			}
			else if (m_StreamHandler.HasEmitter(entityID))
			{
				m_StreamHandler.RemoveEmitter(entityID);
			}

			if (m_OnAudioFinish)
			{
				m_OnAudioFinish(entityID);
			}

			m_AudioEventQueue.erase(m_AudioEventQueue.begin());
		}
	}

} // Mix