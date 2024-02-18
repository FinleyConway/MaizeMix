#pragma once

#include <cstdint>
#include <any>

namespace Mix {

	class AudioFinishCallback
	{
	 public:
		virtual void OnAudioFinish(uint8_t audioSourceID, const std::any& userData) = 0;
	};

} // Mix