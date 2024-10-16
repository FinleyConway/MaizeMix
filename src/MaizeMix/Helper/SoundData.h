#pragma once

#include <cstdint>

namespace Mix {

	struct AudioEventData
	{
		const uint8_t playingID = 0;
		const float stopTime = 0;

		AudioEventData() = default;
		AudioEventData(uint8_t playingID, float stopTime)
			: playingID(playingID), stopTime(stopTime)
		{
		}

		bool operator<(AudioEventData other) const
		{
			if (stopTime == other.stopTime)
			{
				return playingID < other.playingID;
			}

			return stopTime < other.stopTime;
		}
	};

	struct AudioSpecification
	{
		bool loop = false;
		float volume = 0.0f;
		float pitch = 0.0f;

		float x = 0.0f;
		float y = 0.0f;
		float depth = 0.0f;

		float minDistance = 0.0f;
		float maxDistance = 0.0f;

		AudioSpecification() = default;
		AudioSpecification(bool loop, float volume, float pitch, float x, float y, float depth, float minDistance, float maxDistance)
			: loop(loop), volume(volume), pitch(pitch), x(x), y(y), depth(depth), minDistance(minDistance), maxDistance(maxDistance)
		{
		}
	};

} // Mix