#include "MaizeMix/Sounds/SoundHandler.h"

#include <iostream>
#include <limits>

#include "MaizeMix/Helper/SoundData.h"
#include "MaizeMix/Sounds/SoundBuffer.h"
#include "MaizeMix/AudioClip.h"

namespace Mix {

	bool SoundHandler::PlayClip(uint64_t entityID, const SoundBuffer& clip, const AudioSpecification& specification, std::set<AudioEventData>& event, float currentTime)
	{
		const float stopTime = specification.loop ? std::numeric_limits<float>::max() : currentTime + clip.GetDuration().asSeconds();
		const auto [it, success] = event.emplace(entityID, stopTime);

   		if (success)
		{
			// construct audio source and play it
			m_CurrentPlayingAudio.try_emplace(entityID, it, entityID);

			auto& sound = m_CurrentPlayingAudio.at(entityID).sound;
			sound.setBuffer(clip.GetBuffer());
			sound.setVolume(std::clamp(specification.volume, 0.0f, 100.0f));
			sound.setPitch(specification.pitch);
			sound.setLoop(specification.loop);
			sound.setPosition(specification.x, specification.y, specification.depth);
			sound.setMinDistance(specification.minDistance);
			sound.setAttenuation(specification.maxDistance);
			sound.play();

			return true;
		}

		// invalid/duplicate id
		return false;
	}

	bool SoundHandler::PauseClip(uint64_t entityID, std::set<AudioEventData>& event)
	{
		auto& soundData = m_CurrentPlayingAudio.at(entityID);

		if (soundData.IsValid())
		{
			if (soundData.sound.getStatus() == sf::SoundSource::Playing)
			{
				soundData.sound.pause();
				event.erase(soundData.iterator);

				return true;
			}
		}
		else
		{
			HandleInvalid(entityID, soundData, event);
		}

		return false;
	}

	bool SoundHandler::UnPauseClip(uint64_t entityID, std::set<AudioEventData>& event, float currentTime)
	{
		auto& soundData = m_CurrentPlayingAudio.at(entityID);
		auto& sound = soundData.sound;

		if (soundData.IsValid() && sound.getStatus() == sf::SoundSource::Paused)
		{
			sound.play();

			return RequeueAudioClip(entityID, sound, currentTime, soundData, event);
		}

		if (!soundData.IsValid())
		{
			HandleInvalid(entityID, soundData, event);

			return false;
		}

		return false;
	}

	void SoundHandler::StopClip(uint64_t entityID, std::set<AudioEventData>& event, const std::function<void(uint64_t)>& onAudioFinish)
	{
		auto& soundData = m_CurrentPlayingAudio.at(entityID);

		// if only the audio is still valid
		if (soundData.IsValid()) soundData.sound.stop();

		// trigger event handle any outside finished logic
		if (onAudioFinish)
		{
			onAudioFinish(soundData.entity);
		}

		// despite the name, just removes it
		HandleInvalid(entityID, soundData, event);
	}

	bool SoundHandler::SetLoopState(uint64_t entityID, std::set<AudioEventData>& event, float currentTime, bool loop)
	{
		auto& soundData = m_CurrentPlayingAudio.at(entityID);
		auto& sound = soundData.sound;

		if (soundData.IsValid())
		{
			if (sound.getLoop() == loop) return false; // leave function if the same state
			sound.setLoop(loop);

			return RequeueAudioClip(entityID, sound, currentTime, soundData, event);
		}

		return false;
	}

	bool SoundHandler::SetMuteState(uint64_t entityID, bool mute)
	{
		auto& soundData = m_CurrentPlayingAudio.at(entityID);
		const float volume = mute ? 0.0f : soundData.previousVolume;

		// check to see if it's either a music clip or sound effect clip
		if (soundData.IsValid())
		{
			soundData.isMute = mute;
			soundData.previousVolume = soundData.sound.getVolume();
			soundData.sound.setVolume(volume);

			return true;
		}

		return false;
	}

	bool SoundHandler::SetVolume(uint64_t entityID, float volume)
	{
		auto& soundData = m_CurrentPlayingAudio.at(entityID);

		if (soundData.isMute) return false;

		if (soundData.IsValid())
		{
			soundData.sound.setVolume(std::clamp(volume, 0.0f, 100.0f));

			return true;
		}

		return false;
	}

	bool SoundHandler::SetPitch(uint64_t entityID, float pitch)
	{
		auto& soundData = m_CurrentPlayingAudio.at(entityID);

		if (soundData.IsValid())
		{
			soundData.sound.setPitch(pitch);

			return true;
		}

		return false;
	}

	bool SoundHandler::SetPosition(uint64_t entityID, float x, float y, float depth, float minDistance, float maxDistance)
	{
		auto& soundData = m_CurrentPlayingAudio.at(entityID);
		auto& sound = soundData.sound;

		if (soundData.IsValid())
		{

			// sounds should always be at position 0 if its relative, so it acts as 2D
			if (!sound.isRelativeToListener())
			{
				sound.setPosition(x, y, depth);
				sound.setMinDistance(minDistance);
				sound.setMinDistance(maxDistance);

				return true;
			}
		}

		return false;
	}

	bool SoundHandler::SetSpatialState(uint64_t entityID, bool isSpatial)
	{
		auto& soundData = m_CurrentPlayingAudio.at(entityID);

		if (soundData.IsValid())
		{
			auto& sound = soundData.sound;

			if (isSpatial)
			{
				sound.setRelativeToListener(false);
			}
			else
			{
				sound.setRelativeToListener(true);
				sound.setPosition(0, 0, 0);
			}

			return true;
		}

		return false;
	}

	bool SoundHandler::SetAudioOffsetTime(uint64_t entityID, std::set<AudioEventData>& event, float currentTime, float time)
	{
		auto& soundData = m_CurrentPlayingAudio.at(entityID);
		const auto& sound = soundData.sound;

		if (soundData.IsValid())
		{
			// don't need to update the position if they are "equal"
			if (std::abs(time - soundData.previousTimeOffset) < std::numeric_limits<float>::epsilon()) return false;

			soundData.sound.setPlayingOffset(sf::seconds(time));

			return RequeueAudioClip(entityID, sound, currentTime, soundData, event);
		}

		return false;
	}

	float SoundHandler::GetAudioOffsetTime(uint64_t entityID)
	{
		auto& soundData = m_CurrentPlayingAudio.at(entityID);

		if (soundData.IsValid())
		{
			const float offset = soundData.sound.getPlayingOffset().asSeconds();

			soundData.previousTimeOffset = offset;

			return offset;
		}

		return 0.0;
	}

	const SoundHandler::Sound& SoundHandler::GetEmitter(uint64_t entityID) const
	{
		return m_CurrentPlayingAudio.at(entityID);
	}

	void SoundHandler::RemoveEmitter(uint64_t entityID)
	{
		m_CurrentPlayingAudio.erase(entityID);
	}

	bool SoundHandler::HasEmitter(uint64_t entityID) const
	{
		return m_CurrentPlayingAudio.contains(entityID);
	}

	bool SoundHandler::HasEmitters() const
	{
		return !m_CurrentPlayingAudio.empty();
	}

	bool SoundHandler::RequeueAudioClip(uint64_t entityID, const sf::Sound& sound, float currentTime, Sound& soundData, std::set<AudioEventData>& event)
	{
		// calculate the remaining play time
		const float duration = sound.getBuffer()->getDuration().asSeconds();
		const float playingOffset = sound.getPlayingOffset().asSeconds();
		const float playingTimeLeft = duration - playingOffset;
		const float stopTime = currentTime + playingTimeLeft;

		// remove existing event to avoid duplicates
		if (event.contains(*soundData.iterator)) event.erase(soundData.iterator);

		// attempt to reinsert with new stop time
		const auto [it, successful] = event.emplace(entityID, stopTime);
		if (successful)
		{
			soundData.iterator = it;
			return true;
		}

		return false;
	}

	void SoundHandler::HandleInvalid(uint64_t entityID, const Sound& soundData, std::set<AudioEventData>& event)
	{
		if (event.contains(*soundData.iterator))
		{
			event.erase(soundData.iterator);
		}

		if (m_CurrentPlayingAudio.contains(entityID))
		{
			m_CurrentPlayingAudio.erase(entityID);
		}
	}

} // Mix