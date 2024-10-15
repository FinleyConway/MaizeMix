#include "MaizeMix/Engine/EngineHandler/SoundHandler.h"

#include <limits>

#include "MaizeMix/Engine/EngineHandler/SoundData.h"
#include "MaizeMix/Audio/Data/SoundBuffer.h"
#include "MaizeMix/Audio/AudioClip.h"

namespace Mix {

	bool SoundHandler::PlayClip(const SoundBuffer& clip, const AudioSpecification& specification, uint64_t entity, std::set<AudioEventData>& event, uint8_t audioSourceID, float currentTime)
	{
		sf::Sound sound;
		sound.setBuffer(clip.GetBuffer());
		sound.setVolume(std::clamp(specification.volume, 0.0f, 100.0f));
		sound.setPitch(specification.pitch);
		sound.setLoop(specification.loop);
		sound.setPosition(specification.x, specification.y, specification.depth);
		sound.setMinDistance(specification.minDistance);
		sound.setAttenuation(specification.maxDistance);

		const float stopTime = specification.loop ? std::numeric_limits<float>::max() : currentTime + clip.GetDuration().asSeconds();
		const auto [it, success] = event.emplace(audioSourceID, stopTime);

		if (success)
		{
			// construct audio source and play it
			m_CurrentPlayingAudio.try_emplace(audioSourceID, std::move(sound), it, entity);
			m_CurrentPlayingAudio.at(audioSourceID).sound.play();

			return true;
		}

		// invalid/duplicate id
		return false;
	}

	bool SoundHandler::PauseClip(uint8_t playingID, std::set<AudioEventData>& event)
	{
		auto& soundData = m_CurrentPlayingAudio.at(playingID);

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
			HandleInvalid(playingID, soundData, event);
		}

		return false;
	}

	bool SoundHandler::UnPauseClip(uint8_t playingID, std::set<AudioEventData>& event, float currentTime)
	{
		auto& soundData = m_CurrentPlayingAudio.at(playingID);
		auto& sound = soundData.sound;

		if (soundData.IsValid() && sound.getStatus() == sf::SoundSource::Paused)
		{
			sound.play();

			return RequeueAudioClip(sound, playingID, currentTime, soundData, event);
		}

		if (!soundData.IsValid())
		{
			HandleInvalid(playingID, soundData, event);

			return false;
		}

		return false;
	}

	void SoundHandler::StopClip(uint8_t playingID, std::set<AudioEventData>& event, const std::function<void(uint8_t, uint64_t)>& onAudioFinish)
	{
		auto& soundData = m_CurrentPlayingAudio.at(playingID);

		// if only the audio is still valid
		if (soundData.IsValid()) soundData.sound.stop();

		// trigger event handle any outside finished logic
		onAudioFinish(playingID, soundData.entity);

		// despite the name, just removes it
		HandleInvalid(playingID, soundData, event);
	}

	bool SoundHandler::SetLoopState(uint8_t playingID, std::set<AudioEventData>& event, float currentTime, bool loop)
	{
		auto& soundData = m_CurrentPlayingAudio.at(playingID);
		auto& sound = soundData.sound;

		if (soundData.IsValid())
		{
			if (sound.getLoop() == loop) return false; // leave function if the same state
			sound.setLoop(loop);

			return RequeueAudioClip(sound, playingID, currentTime, soundData, event);
		}

		return false;
	}

	bool SoundHandler::SetMuteState(uint8_t playingID, bool mute)
	{
		auto& soundData = m_CurrentPlayingAudio.at(playingID);
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

	bool SoundHandler::SetVolume(uint8_t playingID, float volume)
	{
		auto& soundData = m_CurrentPlayingAudio.at(playingID);

		if (soundData.isMute) return false;

		if (soundData.IsValid())
		{
			soundData.sound.setVolume(std::clamp(volume, 0.0f, 100.0f));

			return true;
		}

		return false;
	}

	bool SoundHandler::SetPitch(uint8_t playingID, float pitch)
	{
		auto& soundData = m_CurrentPlayingAudio.at(playingID);

		if (soundData.IsValid())
		{
			soundData.sound.setVolume(pitch);

			return true;
		}

		return false;
	}

	bool SoundHandler::SetPosition(uint8_t playingID, float x, float y, float depth, float minDistance, float maxDistance)
	{
		auto& soundData = m_CurrentPlayingAudio.at(playingID);

		if (soundData.IsValid())
		{
			auto& sound = soundData.sound;

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

	bool SoundHandler::SetSpatialState(uint8_t playingID, bool isSpatial)
	{
		auto& soundData = m_CurrentPlayingAudio.at(playingID);

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

	bool SoundHandler::SetAudioOffsetTime(uint8_t playingID, std::set<AudioEventData>& event, float currentTime, float time)
	{
		auto& soundData = m_CurrentPlayingAudio.at(playingID);
		const auto& sound = soundData.sound;

		if (soundData.IsValid())
		{
			// don't need to update the position if they are "equal"
			if (std::abs(time - soundData.previousTimeOffset) < std::numeric_limits<float>::epsilon()) return false;

			soundData.sound.setPlayingOffset(sf::seconds(time));

			return RequeueAudioClip(sound, playingID, currentTime, soundData, event);
		}

		return false;
	}

	float SoundHandler::GetAudioOffsetTime(uint8_t playingID)
	{
		auto& soundData = m_CurrentPlayingAudio.at(playingID);
		const float offset = soundData.sound.getPlayingOffset().asSeconds();

		soundData.previousTimeOffset = offset;

		return offset;
	}

	const SoundHandler::Sound& SoundHandler::GetEmitter(uint8_t playingID) const
	{
		return m_CurrentPlayingAudio.at(playingID);
	}

	void SoundHandler::RemoveEmitter(uint8_t playingID)
	{
		m_CurrentPlayingAudio.erase(playingID);
	}

	bool SoundHandler::HasEmitter(uint8_t playingID) const
	{
		return m_CurrentPlayingAudio.contains(playingID);
	}

	bool SoundHandler::HasEmitters() const
	{
		return !m_CurrentPlayingAudio.empty();
	}

	bool SoundHandler::RequeueAudioClip(const sf::Sound& sound, uint8_t playingID, float currentTime, Sound& soundData, std::set<AudioEventData>& event)
	{
		// calculate the remaining play time
		const float duration = sound.getBuffer()->getDuration().asSeconds();
		const float playingOffset = sound.getPlayingOffset().asSeconds();
		const float playingTimeLeft = duration - playingOffset;
		const float stopTime = currentTime + playingTimeLeft;

		// remove existing event to avoid duplicates
		if (event.contains(*soundData.iterator)) event.erase(soundData.iterator);

		// attempt to reinsert with new stop time
		const auto [it, successful] = event.emplace(playingID, stopTime);
		if (successful)
		{
			soundData.iterator = it;
			return true;
		}

		return false;
	}

	void SoundHandler::HandleInvalid(uint8_t playingID, const Sound& soundData, std::set<AudioEventData>& event)
	{
		if (event.contains(*soundData.iterator))
		{
			event.erase(soundData.iterator);
		}

		if (m_CurrentPlayingAudio.contains(playingID))
		{
			m_CurrentPlayingAudio.erase(playingID);
		}
	}

} // Mix