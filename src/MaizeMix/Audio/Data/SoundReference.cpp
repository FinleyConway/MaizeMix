#include "MaizeMix/Audio/Data/SoundReference.h"
#include "MaizeMix/Audio/Music.h"

namespace Mix {

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

	void SoundReference::GetData(std::vector<int16_t>& samples, uint64_t offset)
	{
		uint64_t sampleCount = GetSampleCount();
		uint64_t effectiveOffset = offset % sampleCount;

		samples.resize(sampleCount);

		// set the offset seek position
		m_File.seek(effectiveOffset);

		// read clip
		uint64_t bytesRead = m_File.read(samples.data(), sampleCount - effectiveOffset);

		// if the read length is longer than the clip length, reset the position and read the remaining samples from the start of the clip
		if (bytesRead < sampleCount - effectiveOffset)
		{
			m_File.seek(0);
			bytesRead += m_File.read(samples.data() + bytesRead, sampleCount - bytesRead);
		}

		samples.resize(bytesRead);
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