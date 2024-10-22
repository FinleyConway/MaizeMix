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

		bool PauseAudio(uint64_t entityID);

		bool UnpauseAudio(uint64_t entityID);

		bool StopAudio(uint64_t entityID);

		bool SetAudioLoopState(uint64_t entityID, bool loop);

		bool SetAudioMuteState(uint64_t entityID, bool mute);

		bool SetAudioVolume(uint64_t entityID, float volume);

		bool SetAudioPitch(uint64_t entityID, float pitch);

        bool SetAudioOffsetTime(uint64_t entityID, float time);

		float GetAudioOffsetTime(uint64_t entityID);

		bool SetListenerPosition(float x, float y, float depth) const;

		bool SetGlobalVolume(float volume) const;

		void SetAudioFinishCallback(std::function<void(uint64_t)>&& callback);

		bool HasHitMaxAudioSources() const;

		uint8_t EmitterCount() const;

		void Update(float deltaTime);

	 private:
		sf::Time m_CurrentTime;

		AudioManager m_AudioManager;
		SoundHandler m_SoundHandler;
		StreamHandler m_StreamHandler;
		std::set<AudioEventData> m_AudioEventQueue;
		std::function<void(uint64_t)> m_OnAudioFinish;

		static constexpr uint8_t c_MaxAudioEmitters = 255;
	};

} // Mix