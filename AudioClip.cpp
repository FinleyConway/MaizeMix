#include "AudioClip.h"

AudioClip::AudioClip(size_t clipID, uint32_t channels, float duration, uint32_t frequency, bool stream, AudioClip::LoadState loadState) :
	m_ClipID(clipID), m_Channels(channels), m_Duration(duration), m_Frequency(frequency), m_IsStreaming(stream), m_LoadState(loadState)
{

}

AudioClip::AudioClip(size_t clipID, bool stream, AudioClip::LoadState loadState) :
	m_ClipID(clipID), m_IsStreaming(stream), m_LoadState(loadState)
{

}

bool AudioClip::operator==(const AudioClip& other) const
{
	return other.m_ClipID == m_ClipID;
}