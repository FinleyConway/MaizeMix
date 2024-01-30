#include "AudioClip.h"
#include "AudioRequester.h"

AudioClip::AudioClip(const std::string& audioPath, bool stream)
{
	const auto clip = AudioRequester::LoadClip(audioPath, stream);

	if (clip.wasSuccessful)
	{
		m_ClipID = clip.clipID;
		m_LoadState = LoadState::Loaded;
	}
	else
	{
		m_ClipID = -1;
		m_LoadState = LoadState::Failed;
	}

	m_IsStreaming = stream;
}

AudioClip::~AudioClip()
{
	AudioRequester::UnloadClip(*this);
}

uint32_t AudioClip::GetChannels() const
{
	return AudioRequester::GetClipChannel(*this);
}

float AudioClip::GetDuration() const
{
	return AudioRequester::GetClipDuration(*this);
}

uint32_t AudioClip::GetFrequency() const
{
	return AudioRequester::GetClipFrequency(*this);
}

bool AudioClip::IsLoadInBackground() const
{
	return m_IsStreaming;
}

AudioClip::LoadState AudioClip::GetLoadState() const
{
	return m_LoadState;
}

bool AudioClip::operator==(const AudioClip& other) const
{
	return other.m_ClipID == m_ClipID;
}