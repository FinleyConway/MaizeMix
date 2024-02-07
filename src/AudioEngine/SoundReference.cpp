#include "SoundReference.h"
#include "Music.h"

SoundReference::~SoundReference()
{
	std::set<Music*> music;
	music.swap(m_References);

	for (auto it : music)
	{
		it->ResetReference();
	}
}

bool SoundReference::OpenFromFile(const std::string& filename)
{
	sf::InputSoundFile file;

	if (file.openFromFile(filename))
	{
		m_AudioPath = filename;

		m_Duration = file.getDuration();
		m_ChannelCount = file.getChannelCount();
		m_SampleRate = file.getSampleRate();

		return true;
	}

	return false;
}

sf::Time SoundReference::GetDuration() const
{
	return m_Duration;
}

uint32_t SoundReference::GetChannelCount() const
{
	return m_ChannelCount;
}

uint32_t SoundReference::GetSampleRate() const
{
	return m_SampleRate;
}

void SoundReference::AttachReference(Music* music) const
{
	m_References.insert(music);
}

void SoundReference::DetachReference(Music* music) const
{
	m_References.erase(music);
}
