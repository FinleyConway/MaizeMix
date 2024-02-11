#pragma once

#include <cstdint>

namespace Maize::Mix {

	class AudioFinishCallback
	{
	 public:
		virtual void OnAudioFinish(uint8_t audioSourceID) = 0;
	};

} // Maize
