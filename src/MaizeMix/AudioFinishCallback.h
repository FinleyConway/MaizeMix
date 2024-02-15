#pragma once

#include "AudioSource.h"

namespace Maize::Mix {

	class AudioSource;

	class AudioFinishCallback
	{
	 public:
		virtual void OnAudioFinish(const AudioSource& audioSource) = 0;
	};

} // Maize::Mix
