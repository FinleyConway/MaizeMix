#include "MaizeMix/AudioEngine.h"

#include "MaizeMix/Helper/AudioClips/SoundReference.h"
#include "MaizeMix/Helper/AudioClips/SoundBuffer.h"
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
				return PlayClip(entityID, static_cast<SoundReference&>(*handle), spec, m_AudioEventQueue, m_CurrentTime.asSeconds());
			}

			return PlayClip(entityID, static_cast<SoundBuffer&>(*handle), spec, m_AudioEventQueue, m_CurrentTime.asSeconds());
		}

		return false;
	}

	bool AudioEngine::PauseAudio(uint64_t entityID)
	{
		if (!m_CurrentPlayingAudio.contains(entityID)) return false;

		auto& source = m_CurrentPlayingAudio.at(entityID);

		if (!source.IsValid())
		{
			HandleInvalid(entityID, source.iterator);
			return false;
		}

		return std::visit([&](auto& emitter)
		{
			// pause only if it is playing
			if (emitter.getStatus() == sf::SoundSource::Playing)
			{
				// pause the audio and remove it from the event queue
				emitter.pause();
				m_AudioEventQueue.erase(source.iterator);

				return true;
			}

			return false;
		}, source.source);
	}

	bool AudioEngine::UnpauseAudio(uint64_t entityID)
	{
		if (!m_CurrentPlayingAudio.contains(entityID)) return false;

		auto& source = m_CurrentPlayingAudio.at(entityID);

		if (!source.IsValid())
		{
			HandleInvalid(entityID, source.iterator);
			return false;
		}

		return std::visit([&](auto& emitter)
		{
			// pause only if it is playing
			if (emitter.getStatus() == sf::SoundSource::Paused)
			{
				// pause the audio and remove it from the event queue
				emitter.play();

				return RequeueAudioClip(entityID, source.GetDuration(), emitter.getPlayingOffset().asSeconds(), emitter.getLoop(), m_CurrentTime.asSeconds(), source);
			}

			return false;
		}, source.source);
	}

	bool AudioEngine::StopAudio(uint64_t entityID)
	{
		if (!m_CurrentPlayingAudio.contains(entityID)) return false;

		auto& source = m_CurrentPlayingAudio.at(entityID);

		std::visit([&](auto& emitter) { emitter.stop(); }, source.source);

		// trigger event handle any outside finished logic
		if (m_OnAudioFinish) m_OnAudioFinish(source.entity);

		HandleInvalid(entityID, source.iterator); // despite the name, it just removes it

		return true;
	}

	bool AudioEngine::SetAudioLoopState(uint64_t entityID, bool loop)
	{
		if (!m_CurrentPlayingAudio.contains(entityID)) return false;

		auto& source = m_CurrentPlayingAudio.at(entityID);

		if (!source.IsValid()) return false;

		return std::visit([&](auto& emitter)
		{
			if (emitter.getLoop() == loop) return false; // leave function if the same state
			emitter.setLoop(loop);

			return RequeueAudioClip(entityID, source.GetDuration(), emitter.getPlayingOffset().asSeconds(), emitter.getLoop(), m_CurrentTime.asSeconds(), source);
		}, source.source);
	}

	bool AudioEngine::SetAudioMuteState(uint64_t entityID, bool mute)
	{
		if (!m_CurrentPlayingAudio.contains(entityID)) return false;

		auto& source = m_CurrentPlayingAudio.at(entityID);

		if (!source.IsValid()) return false;

		return std::visit([&](auto& emitter)
		{
			const float volume = mute ? 0.0f : emitter.getVolume();

			source.isMute = mute;
			emitter.setVolume(volume);

			return true;
		}, source.source);
	}

	bool AudioEngine::SetAudioVolume(uint64_t entityID, float volume)
	{
		if (!m_CurrentPlayingAudio.contains(entityID)) return false;

		auto& source = m_CurrentPlayingAudio.at(entityID);

		if (source.isMute) return false;
		if (!source.IsValid()) return false;

		std::visit([&](auto& emitter) { emitter.setVolume(std::clamp(volume, 0.0f, 100.0f)); }, source.source);

		return true;
	}

	bool AudioEngine::SetAudioPitch(uint64_t entityID, float pitch)
	{
		if (!m_CurrentPlayingAudio.contains(entityID)) return false;

		auto& source = m_CurrentPlayingAudio.at(entityID);

		if (!source.IsValid()) return false;

		std::visit([&](auto& emitter) { emitter.setPitch(std::max(0.0001f, pitch)); }, source.source);

		return true;
	}

    bool AudioEngine::SetAudioOffsetTime(uint64_t entityID, float time)
    {
		if (!m_CurrentPlayingAudio.contains(entityID)) return false;

		auto& source = m_CurrentPlayingAudio.at(entityID);

		if (!source.IsValid()) return false;

		return std::visit([&](auto& emitter)
		{
			// don't need to update the position if they are "equal"
			if (std::abs(time - source.previousTimeOffset) < std::numeric_limits<float>::epsilon()) return false;

			emitter.setPlayingOffset(sf::seconds(time));

			return RequeueAudioClip(entityID, source.GetDuration(), emitter.getPlayingOffset().asSeconds(), emitter.getLoop(), m_CurrentTime.asSeconds(), source);
		}, source.source);
    }

    float AudioEngine::GetAudioOffsetTime(uint64_t entityID)
    {
		if (!m_CurrentPlayingAudio.contains(entityID)) return false;

		auto& source = m_CurrentPlayingAudio.at(entityID);

		if (!source.IsValid()) return false;

		return std::visit([&](auto& emitter)
		{
			const float offset = emitter.getPlayingOffset().asSeconds();

			source.previousTimeOffset = offset;

			return offset;
		}, source.source);
    }

	bool AudioEngine::SetListenerPosition(float x, float y, float depth) const
	{
        // causes backend issues if this isn't here, mainly because audio doesn't exist to offset other emitters
		if (!m_CurrentPlayingAudio.empty())
		{
			sf::Listener::setPosition(x, y, depth);

			return true;
		}

		return false;
	}

	bool AudioEngine::SetGlobalVolume(float volume) const
	{
        // causes backend issues if this isn't here, mainly because audio doesn't exist to offset other emitters
		if (!m_CurrentPlayingAudio.empty())
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
			const uint64_t entityID = m_AudioEventQueue.begin()->entityID;

			if (m_CurrentPlayingAudio.contains(entityID))
			{
				m_CurrentPlayingAudio.erase(entityID);

				if (m_OnAudioFinish)
				{
					m_OnAudioFinish(entityID);
				}
			}

			m_AudioEventQueue.erase(m_AudioEventQueue.begin());
		}
	}

	bool AudioEngine::RequeueAudioClip(uint64_t entityID, float duration, float playingOffset, bool isLooping, float currentTime, Source& source)
	{
		// calculate the remaining play time
		const float playingTimeLeft = duration - playingOffset;
		const float stopTime = isLooping ? std::numeric_limits<float>::max() : currentTime + playingTimeLeft;

		// remove existing event to avoid duplicates
		if (m_AudioEventQueue.contains(*source.iterator)) m_AudioEventQueue.erase(source.iterator);

		// attempt to reinsert with new stop time
		const auto [it, successful] = m_AudioEventQueue.emplace(entityID, stopTime);
		if (successful)
		{
			source.iterator = it;
			return true;
		}

		return false;
	}

	void AudioEngine::HandleInvalid(uint64_t entityID, EventIterator it)
	{
		if (m_AudioEventQueue.contains(*it))
		{
			m_AudioEventQueue.erase(it);
		}

		if (m_CurrentPlayingAudio.contains(entityID))
		{
			m_CurrentPlayingAudio.erase(entityID);
		}
	}

} // Mix