#include "AudioSource.h"

namespace Maize::Mix {

	AudioSource::AudioSource(size_t audioSourceID, const AudioSourceDef& def, AudioEngine* engine)
	{
		m_Clip = def.clip;

		m_Loop = def.loop;
		m_Mute = def.mute;

		m_Volume = def.volume;
		m_Pitch = def.pitch;
		m_MinDistance = def.minDistance;
		m_MaxDistance = def.maxDistance;

		m_UserData = def.userData;

		m_Engine = engine;
		m_ID = audioSourceID;
	}

}