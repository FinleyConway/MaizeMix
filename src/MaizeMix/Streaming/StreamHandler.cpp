#include "MaizeMix/Streaming/StreamHandler.h"

#include <limits>

namespace Mix {

	bool StreamHandler::PlayClip(uint64_t entityID, const SoundReference& clip, const AudioSpecification& specification, std::set<AudioEventData>& event, float currentTime)
	{
		const float stopTime = specification.loop ? std::numeric_limits<float>::max() : currentTime + clip.GetDuration().asSeconds();
		const auto& [it, successful] = event.emplace(entityID, stopTime);

		if (successful)
		{
			// construct audio source and play it
			m_CurrentPlayingAudio.try_emplace(entityID, it, entityID);

			auto& stream = m_CurrentPlayingAudio.at(entityID).music;

			if (!stream.Load(clip)) return false; // since this is a wrapper to make it act like sf::Sound we have to load it
			stream.SetVolume(specification.mute ? 0.0f :std::clamp(specification.volume, 0.0f, 100.0f));
			stream.SetPitch(std::max(0.0001f, specification.pitch));
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

	bool StreamHandler::PauseClip(uint64_t entityID, std::set<AudioEventData>& event)
	{
		auto& streamData = m_CurrentPlayingAudio.at(entityID);

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
			HandleInvalid(entityID, streamData, event);
		}

		return false;
	}

	bool StreamHandler::UnPauseClip(uint64_t entityID, std::set<AudioEventData>& event, float currentTime)
	{
		auto& streamData = m_CurrentPlayingAudio.at(entityID);
		auto& stream = streamData.music;

		if (streamData.IsValid() && stream.GetStatus() == sf::SoundSource::Paused)
		{
			stream.Play();

			return RequeueAudioClip(entityID, stream, currentTime, streamData, event);
		}

		if (!streamData.IsValid())
		{
			HandleInvalid(entityID, streamData, event);

			return false;
		}

		return false;
	}

	void StreamHandler::StopClip(uint64_t entityID, std::set<AudioEventData>& event, const std::function<void(uint64_t)>& onAudioFinish)
	{
		auto& streamData = m_CurrentPlayingAudio.at(entityID);

		// if only the audio is still valid
		if (streamData.IsValid()) streamData.music.Stop();

		// trigger event handle any outside finished logic
		if (onAudioFinish)
		{
			onAudioFinish(streamData.entity);
		}

		// despite the name, just removes it
		HandleInvalid(entityID, streamData, event);
	}

	bool StreamHandler::SetLoopState(uint64_t entityID, std::set<AudioEventData>& event, float currentTime, bool loop)
	{
		auto& streamData = m_CurrentPlayingAudio.at(entityID);
		auto& stream = streamData.music;

		if (streamData.IsValid())
		{
			if (stream.GetLoop() == loop) return false; // leave function if the same state
			stream.SetLoop(loop);

			return RequeueAudioClip(entityID, stream, currentTime, streamData, event);
		}

		return false;
	}

	bool StreamHandler::SetMuteState(uint64_t entityID, bool mute)
	{
		auto& streamData = m_CurrentPlayingAudio.at(entityID);
		const float volume = mute ? 0.0f : streamData.music.GetVolume();

		if (streamData.IsValid())
		{
			streamData.isMute = mute;
			streamData.music.SetVolume(volume);

			return true;
		}

		return false;
	}

	bool StreamHandler::SetVolume(uint64_t entityID, float volume)
	{
		auto& streamData = m_CurrentPlayingAudio.at(entityID);

		if (streamData.isMute) return false;

		if (streamData.IsValid())
		{
			streamData.music.SetVolume(std::clamp(volume, 0.0f, 100.0f));

			return true;
		}

		return false;
	}

	bool StreamHandler::SetPitch(uint64_t entityID, float pitch)
	{
		auto& streamData = m_CurrentPlayingAudio.at(entityID);

		if (streamData.IsValid())
		{
			streamData.music.SetPitch(std::max(0.0001f, pitch));

			return true;
		}

		return false;
	}

	bool StreamHandler::SetPosition(uint64_t entityID, float x, float y, float depth, float minDistance, float maxDistance)
	{
		auto& streamData = m_CurrentPlayingAudio.at(entityID);
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

	bool StreamHandler::SetSpatialState(uint64_t entityID, bool isSpatial)
	{
		auto& streamData = m_CurrentPlayingAudio.at(entityID);
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

	bool StreamHandler::SetAudioOffsetTime(uint64_t entityID, std::set<AudioEventData>& event, float currentTime, float time)
	{
		auto& streamData = m_CurrentPlayingAudio.at(entityID);
		auto& stream = streamData.music;

		if (streamData.IsValid())
		{
			// don't need to update the position if they are "equal"
			if (std::abs(time - streamData.previousTimeOffset) < std::numeric_limits<float>::epsilon()) return false;

			stream.SetPlayingOffset(time);

			return RequeueAudioClip(entityID, stream, currentTime, streamData, event);
		}

		return false;
	}

	float StreamHandler::GetAudioOffsetTime(uint64_t entityID)
	{
		auto& streamData = m_CurrentPlayingAudio.at(entityID);

		if (streamData.IsValid())
		{
			const float offset = streamData.music.GetPlayingOffset().asSeconds();

			streamData.previousTimeOffset = offset;

			return offset;
		}

		return 0.0f;
	}

	const StreamHandler::Stream& StreamHandler::GetEmitter(uint64_t entityID) const
	{
		return m_CurrentPlayingAudio.at(entityID);
	}

	void StreamHandler::RemoveEmitter(uint64_t entityID)
	{
		m_CurrentPlayingAudio.erase(entityID);
	}

	bool StreamHandler::HasEmitter(uint64_t entityID) const
	{
		return m_CurrentPlayingAudio.contains(entityID);
	}

	bool StreamHandler::HasEmitters() const
	{
		return !m_CurrentPlayingAudio.empty();
	}

	bool StreamHandler::RequeueAudioClip(uint64_t entityID, const Music& stream, float currentTime, Stream& streamData, std::set<AudioEventData>& event)
	{
		// calculate the remaining play time
		const float duration = stream.GetReference()->GetDuration().asSeconds();
		const float playingOffset = stream.GetPlayingOffset().asSeconds();
		const float playingTimeLeft = duration - playingOffset;
		const float stopTime = stream.GetLoop() ? std::numeric_limits<float>::max() : currentTime + playingTimeLeft;

		// remove existing event to avoid duplicates
		if (event.contains(*streamData.iterator)) event.erase(streamData.iterator);

		// attempt to reinsert with new stop time
		const auto [it, successful] = event.emplace(entityID, stopTime);
		if (successful)
		{
			streamData.iterator = it;
			return true;
		}

		return false;
	}

	void StreamHandler::HandleInvalid(uint64_t entityID, const Stream& streamData, std::set<AudioEventData>& event)
	{

		if (event.contains(*streamData.iterator))
		{
			event.erase(streamData.iterator);
		}

		if (m_CurrentPlayingAudio.contains(entityID))
		{
			m_CurrentPlayingAudio.erase(entityID);
		}
	}

} // Mix