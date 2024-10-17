#pragma once

#include <SFML/Audio.hpp>

#include <functional>
#include <memory>
#include <set>

#include "MaizeMix/Streaming/StreamHandler.h"
#include "MaizeMix/Helper/AudioManager.h"
#include "MaizeMix/Sounds/SoundHandler.h"
#include "MaizeMix/Helper/SoundData.h"
#include "MaizeMix/AudioClip.h"

namespace Mix {

	class AudioClip;
	class Music;
	class AudioFinishCallback;

	class AudioEngine
	{
	 public:
		AudioClip CreateClip(const std::string& filePath, bool stream);

		void RemoveClip(AudioClip& clip);

		bool PlayAudio(uint64_t entityID, const AudioClip& clip, const AudioSpecification& spec);
		void PauseAudio(uint64_t entityID);
		void UnpauseAudio(uint64_t entityID);
		void StopAudio(uint64_t entityID);

		void SetAudioLoopState(uint64_t entityID, bool loop);
		void SetAudioMuteState(uint64_t entityID, bool mute);
		void SetAudioVolume(uint64_t entityID, float volume);
		void SetAudioPitch(uint64_t entityID, float pitch);
		void SetAudioPosition(uint64_t entityID, float x, float y, float depth, float minDistance, float maxDistance);
		void SetSpatialMode(uint64_t entityID, bool isSpatial);
        void SetAudioOffsetTime(uint64_t entityID, float time);

		float GetAudioOffsetTime(uint64_t entityID);

		void SetListenerPosition(float x, float y, float depth) const;
		void SetGlobalVolume(float volume) const;

		void SetAudioFinishCallback(std::function<void(uint64_t)>&& callback);

		void Update(float deltaTime);

		bool HasHitMaxAudioSources() const;

	 private:
		sf::Time m_CurrentTime;

		SoundHandler m_SoundHandler;
		StreamHandler m_StreamHandler;
		AudioManager m_AudioManager;

		std::set<AudioEventData> m_AudioEventQueue;
		std::function<void(uint64_t)> m_OnAudioFinish;

		static constexpr uint8_t c_MaxAudioEmitters = 250;
	};

} // Mix