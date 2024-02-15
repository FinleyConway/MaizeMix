#pragma once

#include <SFML/Audio.hpp>

#include <unordered_map>
#include <variant>
#include <memory>
#include <set>

#include "SoundReference.h"

namespace Maize::Mix {

	class AudioClip;
	class AudioSourceDef;
	class AudioSource;
	class Music;
	class AudioFinishCallback;

	class AudioEngine
	{
	 public:
		AudioClip CreateClip(const std::string& audioPath, bool stream);
		void DestroyClip(AudioClip& clip);

		size_t CreateAudioSource(const AudioSourceDef& audioSourceDef);
		void DestroyAudioSource(size_t audioSourceID);

		void PlayAudio(size_t audioSourceID, float x = 0, float y = 0, float z = 0);
        void PauseAudio(uint8_t audioSourceID);
        void UnpauseAudio(uint8_t audioSourceID);
        void StopAudio(uint8_t audioSourceID);

		void SetListenerPosition(float x, float y, float depth);
		void SetGlobalVolume(float volume);

		void SetAudioFinishCallback(AudioFinishCallback* callback);

		void Update(float deltaTime);

	 private:
		struct SoundEventData
		{
			const size_t playingID = 0;
			const float stopTime = 0;

			SoundEventData() = default;
			SoundEventData(size_t audioSourceID, float stopTime) : playingID(audioSourceID), stopTime(stopTime) { }

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
			float previousVolume = 0;

			Audio() = default;
			explicit Audio(const std::shared_ptr<Music>& audio) : sound(audio) { }
			explicit Audio(const sf::Sound& audio) : sound(audio) { }
		};

	 private:
		float LimitVolume(float volume) const;
		bool HasHitMaxAudioSources() const;
		float GetPlayingOffset(const std::variant<sf::Sound, std::shared_ptr<Music>>& soundVariant);

		uint8_t PlayAudioClip(AudioClip& clip, float volume, float pitch, bool loop, float x, float y, float depth, float minDistance, float maxDistance);
		uint8_t PlayStreamedAudioClip(AudioClip& clip, float volume, float pitch, bool loop, float x, float y, float depth, float minDistance, float maxDistance);

		uint8_t GetNextID();
		void ReturnID(uint8_t playingID);

	 private:
		sf::Time m_CurrentTime;

		std::unordered_map<size_t, sf::SoundBuffer> m_SoundBuffers;
		std::unordered_map<size_t, SoundReference> m_SoundReferences;

		std::unordered_map<size_t, AudioSource> m_AudioSources;

		std::unordered_map<uint8_t, Audio> m_CurrentPlayingSounds;
		std::set<SoundEventData> m_AudioEventQueue;

		std::vector<uint8_t> m_UnusedIDs;

		AudioFinishCallback* m_Callback = nullptr;

		static constexpr uint8_t c_MaxAudioEmitters = 250;
		static constexpr uint8_t c_InvalidClip = 0;
		static constexpr uint8_t c_InvalidAudioSource = 0;
	};

}