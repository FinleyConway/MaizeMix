#pragma once

#include <set>
#include <memory>
#include <functional>
#include <unordered_map>

#include "MaizeMix/Streaming/Music.h"
#include "MaizeMix/Streaming/SoundReference.h"
#include "MaizeMix/Helper/SoundData.h"

namespace Mix {

	class StreamHandler
	{
	struct Stream;

	public:
		bool PlayClip(const SoundReference& clip, const AudioSpecification& specification, uint64_t entity, std::set<AudioEventData>& event, uint8_t audioSourceID, float currentTime);

		bool PauseClip(uint8_t playingID, std::set<AudioEventData>& event);

		bool UnPauseClip(uint8_t playingID, std::set<AudioEventData>& event, float currentTime);

		void StopClip(uint8_t playingID, std::set<AudioEventData>& event, const std::function<void(uint8_t, uint64_t)>& onAudioFinish);

		bool SetLoopState(uint8_t playingID, std::set<AudioEventData>& event, float currentTime, bool loop);

		bool SetMuteState(uint8_t playingID, bool mute);

		bool SetVolume(uint8_t playingID, float volume);

		bool SetPitch(uint8_t playingID, float pitch);

		bool SetPosition(uint8_t playingID, float x, float y, float depth, float minDistance, float maxDistance);

		bool SetSpatialState(uint8_t playingID, bool isSpatial);

		bool SetAudioOffsetTime(uint8_t playingID, std::set<AudioEventData>& event, float currentTime, float time);

		float GetAudioOffsetTime(uint8_t playingID);

		const Stream& GetEmitter(uint8_t playingID) const;

		void RemoveEmitter(uint8_t playingID);

		bool HasEmitter(uint8_t playingID) const;

		bool HasEmitters() const;

	private:
		bool RequeueAudioClip(const Music& stream, uint8_t playingID, float currentTime, Stream& streamData, std::set<AudioEventData>& event);

		void HandleInvalid(uint8_t playingID, const Stream& streamData, std::set<AudioEventData>& event);

	private:
		using EventIterator = std::set<AudioEventData>::iterator;

		struct Stream
		{
			Music music;

			uint64_t entity = 0;
			EventIterator iterator;

			bool isMute = false;
			float previousVolume = 0;
			float previousTimeOffset = 0;

			Stream(EventIterator event, uint64_t entity)
					: entity(entity), iterator(event) { }

			bool IsValid() const { return music.GetReference() != nullptr; }
		};

	private:
		std::unordered_map<uint8_t, Stream> m_CurrentPlayingAudio;

		static constexpr uint8_t c_InvalidAudioSource = 0;
	};

} // Mix