#pragma once

#include <cstdint>

namespace Mix {

	struct AudioEventData
	{
		const uint64_t entityID = 0;
		const float stopTime = 0;

		AudioEventData() = default;
		AudioEventData(uint64_t entityID, float stopTime)
			: entityID(entityID), stopTime(stopTime)
		{
		}

		bool operator<(AudioEventData other) const
		{
			if (stopTime == other.stopTime)
			{
				return entityID < other.entityID;
			}

			return stopTime < other.stopTime;
		}
	};

	struct AudioSpecification
	{
		bool mute = false;
		bool loop = false;
		float volume = 0.0f;
		float pitch = 0.0f;

		float x = 0.0f;
		float y = 0.0f;
		float depth = 0.0f;

		float minDistance = 0.0f;
		float maxDistance = 0.0f;

		AudioSpecification() = default;
		AudioSpecification(bool loop, bool mute, float volume, float pitch, float x, float y, float depth, float minDistance, float maxDistance)
			: mute(mute), loop(loop), volume(volume), pitch(pitch), x(x), y(y), depth(depth), minDistance(minDistance), maxDistance(maxDistance)
		{
		}
	};

} // Mix