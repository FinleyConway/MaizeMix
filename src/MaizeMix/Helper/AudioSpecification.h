#pragma once

namespace Mix {

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