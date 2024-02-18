#pragma once

#include "MaizeMix/MaizeMix.h"

struct PlayAudioSourceTag { };
struct PauseAudioSourceTag { };
struct UnPauseAudioSourceTag { };
struct StopAudioSourceTag { };
struct PlayingAudioSourceTag { uint8_t playingID = 0; };

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