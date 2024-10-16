#include "MaizeMix/Engine/EngineHandler/StreamHandler.h"

#include <limits>

namespace Mix {

	bool StreamHandler::PlayClip(const SoundReference& clip, const AudioSpecification& specification, uint64_t entity, std::set<AudioEventData>& event, uint8_t audioSourceID, float currentTime)
	{
		const float stopTime = specification.loop ? std::numeric_limits<float>::max() : currentTime + clip.GetDuration().asSeconds();
		const auto& [it, successful] = event.emplace(audioSourceID, stopTime);

		if (successful)
		{
			// construct audio source and play it
			m_CurrentPlayingAudio.try_emplace(audioSourceID, it, entity);

			auto& stream = m_CurrentPlayingAudio.at(audioSourceID).music;

			if (!stream.Load(clip)) return false; // since this is a wrapper to make it act like sf::Sound we have to load it
			stream.SetVolume(std::clamp(specification.volume, 0.0f, 100.0f));
			stream.SetPitch(specification.pitch);
			stream.SetLoop(specification.loop);
			stream.SetPosition(specification.x, specification.y, specification.depth);
			stream.SetMinDistance(specification.minDistance);
			stream.SetMaxDistance(specification.maxDistance);
			stream.Play();

			return true;
		}

		// duplicate id
		return false;
	}

	bool StreamHandler::PauseClip(uint8_t playingID, std::set<AudioEventData>& event)
	{
		auto& streamData = m_CurrentPlayingAudio.at(playingID);

		if (streamData.IsValid())
		{
			if (streamData.music.GetStatus() == sf::SoundSource::Playing)
			{
				streamData.music.Pause();
				event.erase(streamData.iterator);

				return true;
			}
		}
		else
		{
			HandleInvalid(playingID, streamData, event);
		}

		return false;
	}

	bool StreamHandler::UnPauseClip(uint8_t playingID, std::set<AudioEventData>& event, float currentTime)
	{
		auto& streamData = m_CurrentPlayingAudio.at(playingID);
		auto& stream = streamData.music;

		if (streamData.IsValid() && stream.GetStatus() == sf::SoundSource::Paused)
		{
			stream.Play();

			return RequeueAudioClip(stream, playingID, currentTime, streamData, event);
		}

		if (!streamData.IsValid())
		{
			HandleInvalid(playingID, streamData, event);

			return false;
		}

		return false;
	}

	void StreamHandler::StopClip(uint8_t playingID, std::set<AudioEventData>& event, const std::function<void(uint8_t, uint64_t)>& onAudioFinish)
	{
		auto& streamData = m_CurrentPlayingAudio.at(playingID);

		// if only the audio is still valid
		if (streamData.IsValid()) streamData.music.Stop();

		// trigger event handle any outside finished logic
		onAudioFinish(playingID, streamData.entity);

		// despite the name, just removes it
		HandleInvalid(playingID, streamData, event);
	}

	bool StreamHandler::SetLoopState(uint8_t playingID, std::set<AudioEventData>& event, float currentTime, bool loop)
	{
		auto& streamData = m_CurrentPlayingAudio.at(playingID);
		auto& stream = streamData.music;

		if (streamData.IsValid())
		{
			if (stream.GetLoop() == loop) return false; // leave function if the same state
			stream.SetLoop(loop);

			return RequeueAudioClip(stream, playingID, currentTime, streamData, event);
		}

		return false;
	}

	bool StreamHandler::SetMuteState(uint8_t playingID, bool mute)
	{
		auto& streamData = m_CurrentPlayingAudio.at(playingID);
		const float volume = mute ? 0.0f : streamData.previousVolume;

		// check to see if it's either a music clip or sound effect clip
		if (streamData.IsValid())
		{
			streamData.isMute = mute;
			streamData.previousVolume = streamData.music.GetVolume();
			streamData.music.SetVolume(volume);

			return true;
		}

		return false;
	}

	bool StreamHandler::SetVolume(uint8_t playingID, float volume)
	{
		auto& streamData = m_CurrentPlayingAudio.at(playingID);

		if (streamData.isMute) return false;

		if (streamData.IsValid())
		{
			streamData.music.SetVolume(std::clamp(volume, 0.0f, 100.0f));

			return true;
		}

		return false;
	}

	bool StreamHandler::SetPitch(uint8_t playingID, float pitch)
	{
		auto& streamData = m_CurrentPlayingAudio.at(playingID);

		if (streamData.IsValid())
		{
			streamData.music.SetPitch(pitch);

			return true;
		}

		return false;
	}

	bool StreamHandler::SetPosition(uint8_t playingID, float x, float y, float depth, float minDistance, float maxDistance)
	{
		auto& streamData = m_CurrentPlayingAudio.at(playingID);
		auto& stream = streamData.music;

		if (streamData.IsValid())
		{
			// sounds should always be at position 0 if its relative, so it acts as 2D
			if (!stream.IsRelativeToListener())
			{
				stream.SetPosition(x, y, depth);
				stream.SetMinDistance(minDistance);
				stream.SetMinDistance(maxDistance);

				return true;
			}
		}

		return false;
	}

	bool StreamHandler::SetSpatialState(uint8_t playingID, bool isSpatial)
	{
		auto& streamData = m_CurrentPlayingAudio.at(playingID);
		auto& stream = streamData.music;

		if (streamData.IsValid())
		{
			if (isSpatial)
			{
				stream.SetRelativeToListener(false);
			}
			else
			{
				stream.SetRelativeToListener(true);
				stream.SetPosition(0, 0, 0);
			}

			return true;
		}

		return false;
	}

	bool StreamHandler::SetAudioOffsetTime(uint8_t playingID, std::set<AudioEventData>& event, float currentTime, float time)
	{
		auto& streamData = m_CurrentPlayingAudio.at(playingID);
		auto& stream = streamData.music;

		if (streamData.IsValid())
		{
			// don't need to update the position if they are "equal"
			if (std::abs(time - streamData.previousTimeOffset) < std::numeric_limits<float>::epsilon()) return false;

			stream.SetPlayingOffset(time);

			return RequeueAudioClip(stream, playingID, currentTime, streamData, event);
		}

		return false;
	}

	float StreamHandler::GetAudioOffsetTime(uint8_t playingID)
	{
		auto& streamData = m_CurrentPlayingAudio.at(playingID);

		if (streamData.IsValid())
		{
			const float offset = streamData.music.GetPlayingOffset().asSeconds();

			streamData.previousTimeOffset = offset;

			return offset;
		}

		return 0.0f;
	}

	const StreamHandler::Stream& StreamHandler::GetEmitter(uint8_t playingID) const
	{
		return m_CurrentPlayingAudio.at(playingID);
	}

	void StreamHandler::RemoveEmitter(uint8_t playingID)
	{
		m_CurrentPlayingAudio.erase(playingID);
	}

	bool StreamHandler::HasEmitter(uint8_t playingID) const
	{
		return m_CurrentPlayingAudio.contains(playingID);
	}

	bool StreamHandler::HasEmitters() const
	{
		return !m_CurrentPlayingAudio.empty();
	}

	bool StreamHandler::RequeueAudioClip(const Music& stream, uint8_t playingID, float currentTime, Stream& streamData, std::set<AudioEventData>& event)
	{
		// calculate the remaining play time
		const float duration = stream.GetReference()->GetDuration().asSeconds();
		const float playingOffset = stream.GetPlayingOffset().asSeconds();
		const float playingTimeLeft = duration - playingOffset;
		const float stopTime = currentTime + playingTimeLeft;

		// remove existing event to avoid duplicates
		if (event.contains(*streamData.iterator)) event.erase(streamData.iterator);

		// attempt to reinsert with new stop time
		const auto [it, successful] = event.emplace(playingID, stopTime);
		if (successful)
		{
			streamData.iterator = it;
			return true;
		}

		return false;
	}

	void StreamHandler::HandleInvalid(uint8_t playingID, const Stream& streamData, std::set<AudioEventData>& event)
	{

		if (event.contains(*streamData.iterator))
		{
			event.erase(streamData.iterator);
		}

		if (m_CurrentPlayingAudio.contains(playingID))
		{
			m_CurrentPlayingAudio.erase(playingID);
		}
	}

} // Mix