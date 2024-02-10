#pragma once

#include "MaizeMix.h"

namespace Maize {

	struct PlayAudioSourceTag { };
    struct PauseAudioSourceTag { };
	struct StopAudioSourceTag { };
	struct PlayingAudioSourceTag { const uint8_t audioSourceID = 0; };

	struct PositionComponent
	{
		float x = 0;
		float y = 0;
	};

	struct AudioSourceComponent
	{
		Mix::AudioClip clip;

		bool loop = false;
		bool mute = false;
		bool spatialize = false;

		float volume = 100;
		float pitch = 1;
		float time = 0;
		float minDistance = 1.0f;
		float maxDistance = 500.0f;
	};

} // Maize