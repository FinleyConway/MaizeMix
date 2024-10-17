#pragma once

#include <SFML/Audio.hpp>

#include <set>
#include <cstdint>
#include <functional>
#include <unordered_map>

namespace Mix {

	class AudioClip;
	class SoundBuffer;
	struct AudioEventData;
	struct AudioSpecification;

	class SoundHandler
	{
	struct Sound;

	public:
		bool PlayClip(uint64_t entityID, const SoundBuffer& clip, const AudioSpecification& specification, std::set<AudioEventData>& event, float currentTime);

		bool PauseClip(uint64_t entityID, std::set<AudioEventData>& event);

		bool UnPauseClip(uint64_t entityID, std::set<AudioEventData>& event, float currentTime);

		void StopClip(uint64_t entityID, std::set<AudioEventData>& event, const std::function<void(uint64_t)>& onAudioFinish);

		bool SetLoopState(uint64_t entityID, std::set<AudioEventData>& event, float currentTime, bool loop);

		bool SetMuteState(uint64_t entityID, bool mute);

		bool SetVolume(uint64_t entityID, float volume);

		bool SetPitch(uint64_t entityID, float pitch);

		bool SetPosition(uint64_t entityID, float x, float y, float depth, float minDistance, float maxDistance);

		bool SetSpatialState(uint64_t entityID, bool isSpatial);

		bool SetAudioOffsetTime(uint64_t entityID, std::set<AudioEventData>& event, float currentTime, float time);

		float GetAudioOffsetTime(uint64_t entityID);

		const Sound& GetEmitter(uint64_t entityID) const;

		void RemoveEmitter(uint64_t entityID);

		bool HasEmitter(uint64_t entityID) const;

		bool HasEmitters() const;

	private:
		bool RequeueAudioClip(uint64_t entityID, const sf::Sound& sound, float currentTime, Sound& soundData, std::set<AudioEventData>& event);

		void HandleInvalid(uint64_t entityID, const Sound& soundData, std::set<AudioEventData>& event);

	private:
		using EventIterator = std::set<AudioEventData>::iterator;

		struct Sound
		{
			sf::Sound sound;

			uint64_t entity = 0;
			EventIterator iterator;

			bool isMute = false;
			float previousTimeOffset = 0;

			Sound(EventIterator event, uint64_t entity)
					: entity(entity), iterator(event) { }

			bool IsValid() const { return sound.getBuffer() != nullptr; }
		};

	private:
		std::unordered_map<uint64_t, Sound> m_CurrentPlayingAudio;
	};

} // Mix