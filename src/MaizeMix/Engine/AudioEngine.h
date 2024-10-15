#pragma once

#include <SFML/Audio.hpp>

#include <unordered_map>
#include <functional>
#include <memory>
#include <set>

#include "AudioManager.h"
#include "EngineHandler/SoundHandler.h"
#include "MaizeMix/Audio/AudioClip.h"
#include "MaizeMix/Engine/EngineHandler/SoundData.h"

namespace Mix {

	class AudioClip;
	class Music;
	class AudioFinishCallback;

	class AudioEngine
	{
	 public:
		AudioClip CreateClip(const std::string& filePath, bool stream)
		{
			return m_AudioManager.CreateClip(filePath, stream);
		}

		void RemoveClip(AudioClip& clip)
		{
			m_AudioManager.DestroyClip(clip);
		}

		uint8_t PlayAudio(const AudioClip& clip, const AudioSpecification& spec, uint64_t entity);
		void PauseAudio(uint8_t playingID);
		void UnpauseAudio(uint8_t playingID);
		void StopAudio(uint8_t playingID);

		void SetAudioLoopState(uint8_t playingID, bool loop);
		void SetAudioMuteState(uint8_t playingID, bool mute);
		void SetAudioVolume(uint8_t playingID, float volume);
		void SetAudioPitch(uint8_t playingID, float pitch);
		void SetAudioPosition(uint8_t playingID, float x, float y, float depth, float minDistance, float maxDistance);
		void SetSpatialMode(uint8_t playingID, bool isSpatial);
        void SetAudioOffsetTime(uint8_t playingID, float time);

		float GetAudioOffsetTime(uint8_t playingID);

		void SetListenerPosition(float x, float y, float depth) const;
		void SetGlobalVolume(float volume) const;

		void SetAudioFinishCallback(std::function<void(uint8_t, uint64_t)>&& callback);

		void Update(float deltaTime);

	 private:
		bool HasHitMaxAudioSources() const;

		uint8_t GetNextID();
		void ReturnID(uint8_t audioSourceID);

	 private:
		sf::Time m_CurrentTime;

		SoundHandler m_SoundHandler;
		AudioManager m_AudioManager;

		std::set<AudioEventData> m_AudioEventQueue;
		std::vector<uint8_t> m_UnusedIDs;

		std::function<void(uint8_t, uint64_t)> m_OnAudioFinish;

		static constexpr uint8_t c_MaxAudioEmitters = 250;
		static constexpr uint8_t c_InvalidClip = 0;
		static constexpr uint8_t c_InvalidAudioSource = 0;
	};

} // Mix