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
		bool PlayClip(uint64_t entityID, const SoundReference& clip, const AudioSpecification& specification, std::set<AudioEventData>& event, float currentTime);

		bool PauseClip(uint64_t entityID, std::set<AudioEventData>& event);

		bool UnPauseClip(uint64_t entityID, std::set<AudioEventData>& event, float currentTime);

		void StopClip(uint64_t entityID, std::set<AudioEventData>& event, const std::function<void(uint64_t)>& onAudioFinish);

		bool SetLoopState(uint64_t entityID, std::set<AudioEventData>& event, float currentTime, bool loop);

		bool SetMuteState(uint64_t entityID, bool mute);

		bool SetVolume(uint64_t entityID, float volume);

		bool SetPitch(uint64_t entityID, float pitch);

		bool SetAudioOffsetTime(uint64_t entityID, std::set<AudioEventData>& event, float currentTime, float time);

		float GetAudioOffsetTime(uint64_t entityID);

		const Stream& GetEmitter(uint64_t entityID) const;

		void RemoveEmitter(uint64_t entityID);

		bool HasEmitter(uint64_t entityID) const;

		bool HasEmitters() const;

	private:
		static bool RequeueAudioClip(uint64_t entityID, const Music& stream, float currentTime, Stream& streamData, std::set<AudioEventData>& event);

		void HandleInvalid(uint64_t entityID, const Stream& streamData, std::set<AudioEventData>& event);

	private:
		using EventIterator = std::set<AudioEventData>::iterator;

		struct Stream
		{
			Music music;

			uint64_t entity = 0;
			EventIterator iterator;

			bool isMute = false;
			float previousTimeOffset = 0;

			Stream(EventIterator event, uint64_t entity)
					: entity(entity), iterator(event) { }

			bool IsValid() const { return music.GetReference() != nullptr; }
		};

	private:
		std::unordered_map<uint64_t, Stream> m_CurrentPlayingAudio;
	};

} // Mix