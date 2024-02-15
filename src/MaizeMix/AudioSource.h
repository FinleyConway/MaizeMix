#pragma once

#include <any>
#include <cstddef>

#include "AudioClip.h"

namespace Maize::Mix {

	class AudioEngine;

	struct AudioSourceDef
	{
		AudioSourceDef() = default;

		AudioClip clip;

		bool loop = false;
		bool mute = false;

		float volume = 100;
		float pitch = 1;
		float minDistance = 1.0f;
		float maxDistance = 500.0f;

		std::any userData;
	};

	class AudioSource
	{
	 public:
		void Play();
		void PlayAtPosition(float x, float y, float z);
		void Pause();
		void UnPause();
		void Stop();

	 private:
		friend class AudioEngine;

		AudioSource(size_t audioSourceID, const AudioSourceDef& def, AudioEngine* engine);

		AudioClip m_Clip;
		uint8_t m_PlayingID = 0;

		bool m_Loop = false;
		bool m_Mute = false;

		float m_Volume = 100;
		float m_Pitch = 1;
		float m_MinDistance = 1.0f;
		float m_MaxDistance = 500.0f;

		std::any m_UserData;

		AudioEngine* m_Engine = nullptr;
		size_t m_ID = 0;
	};

} // Maize::Mix
