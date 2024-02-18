#pragma once

#include "SFML/Audio.hpp"

#include <unordered_map>
#include <variant>
#include <memory>
#include <any>
#include <set>

#include "../Audio/SoundReference.h"

namespace Mix {

	class AudioClip;
	class Music;
	class AudioFinishCallback;

	class AudioEngine
	{
	 public:
		AudioClip CreateClip(const std::string& audioPath, bool stream);
		void DestroyClip(AudioClip& clip);

		uint8_t PlayAudio(AudioClip& clip, float volume, float pitch, bool loop, const std::any& userData, float x = 0, float y = 0, float depth = 0, float minDistance = 0, float maxDistance = 0);
		void PauseAudio(uint8_t playingID);
		void UnpauseAudio(uint8_t playingID);
		void StopAudio(uint8_t playingID);

		void SetAudioLoopState(uint8_t playingID, bool loop);
		void SetAudioMuteState(uint8_t playingID, bool mute);
		void SetAudioVolume(uint8_t playingID, float volume);
		void SetAudioPitch(uint8_t playingID, float pitch);
		void SetAudioPosition(uint8_t playingID, float x, float y, float depth, float minDistance, float maxDistance);
		void SetAudioPlayback(uint8_t playingID, float seconds);
		void SetSpatializationMode(uint8_t playingID, bool isSpatialization);

		float GetAudioOffsetTime(uint8_t playingID);

		void SetListenerPosition(float x, float y, float depth);
		void SetGlobalVolume(float volume);

		void SetAudioFinishCallback(AudioFinishCallback* callback);

		void Update(float deltaTime);

	 private:
		struct SoundEventData
		{
			const uint8_t playingID = 0;
			const float stopTime = 0;

			SoundEventData() = default;
			SoundEventData(uint8_t playingID, float stopTime)
				: playingID(playingID), stopTime(stopTime)
			{
			}

			bool operator<(const SoundEventData& other) const
			{
				if (stopTime == other.stopTime)
				{
					return playingID < other.playingID;
				}

				return stopTime < other.stopTime;
			}

		};

		struct Audio
		{
			std::variant<sf::Sound, std::shared_ptr<Music>> sound;
			const SoundEventData* event = nullptr;
			const std::any userData;
			float previousVolume = 0;

			Audio() = default;
			Audio(const std::shared_ptr<Music>& audio, const SoundEventData* event, const std::any& userData)
				: sound(audio), event(event), userData(userData)
			{
			}
			Audio(const sf::Sound& audio, const SoundEventData* event, const std::any& userData)
				: sound(audio), event(event), userData(userData)
			{
			}
		};

	 private:
		float LimitVolume(float volume) const;
		bool HasHitMaxAudioSources() const;
		float GetPlayingOffset(const std::variant<sf::Sound, std::shared_ptr<Music>>& soundVariant);
		float GetDuration(const std::variant<sf::Sound, std::shared_ptr<Music>>& soundVariant);

		uint8_t PlayAudioClip(AudioClip& clip, float volume, float pitch, bool loop, float x, float y, float depth, float minDistance, float maxDistance, const std::any& userData);
		uint8_t PlayStreamedAudioClip(AudioClip& clip, float volume, float pitch, bool loop, float x, float y, float depth, float minDistance, float maxDistance, const std::any& userData);

		uint8_t GetNextID();
		void ReturnID(uint8_t audioSourceID);

	 private:
		sf::Time m_CurrentTime;

		std::unordered_map<size_t, sf::SoundBuffer> m_SoundBuffers;
		std::unordered_map<size_t, SoundReference> m_SoundReferences;

		std::unordered_map<uint8_t, Audio> m_CurrentPlayingAudio;
		std::set<SoundEventData> m_AudioEventQueue;

		std::vector<uint8_t> m_UnusedIDs;

		AudioFinishCallback* m_Callback = nullptr;

		static constexpr uint8_t c_MaxAudioEmitters = 250;
		static constexpr uint8_t c_InvalidClip = 0;
		static constexpr uint8_t c_InvalidAudioSource = 0;
	};

} // Mix