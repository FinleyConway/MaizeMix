#include "MaizeMix/Helper/AudioClips/SoundReference.h"
#include "MaizeMix/Helper/Music.h"

namespace Mix {

	SoundReference::~SoundReference()
	{
		std::set<Music*> music;
		music.swap(m_References);

		for (auto* it : music)
		{
			it->resetReference();
		}
	}

	bool SoundReference::OpenFromFile(const std::string& filename)
	{
		if (m_File.openFromFile(filename))
		{
			m_AudioPath = filename;

			m_Duration = m_File.getDuration();
			m_ChannelCount = m_File.getChannelCount();
			m_SampleRate = m_File.getSampleRate();
			m_SampleCount = m_File.getSampleOffset();

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

	uint64_t SoundReference::GetSampleCount() const
	{
		return m_SampleCount;
	}

	void SoundReference::AttachReference(Music* music) const
	{
		m_References.insert(music);
	}

	void SoundReference::DetachReference(Music* music) const
	{
		m_References.erase(music);
	}

} // Mix