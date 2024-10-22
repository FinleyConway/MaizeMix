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

		bool operator<(const AudioEventData& other) const
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

		AudioSpecification() = default;
		AudioSpecification(bool loop, bool mute, float volume, float pitch)
			: mute(mute), loop(loop), volume(volume), pitch(pitch)
		{
		}
	};

} // Mix