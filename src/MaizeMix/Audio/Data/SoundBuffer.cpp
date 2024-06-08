#include "MaizeMix/Audio/Data/SoundBuffer.h"

namespace Mix {

	bool SoundBuffer::OpenFromFile(const std::string& filename)
	{
		return m_Buffer.loadFromFile(filename);
	}

	sf::Time SoundBuffer::GetDuration() const
	{
		return m_Buffer.getDuration();
	}

	uint32_t SoundBuffer::GetChannelCount() const
	{
		return m_Buffer.getChannelCount();
	}

	uint32_t SoundBuffer::GetSampleRate() const
	{
		return m_Buffer.getSampleRate();
	}

	uint64_t SoundBuffer::GetSampleCount() const
	{
		return m_Buffer.getSampleCount();
	}

	void SoundBuffer::GetData(std::vector<int16_t>& samples, uint64_t offset)
	{
		const int16_t* bufferSamples = m_Buffer.getSamples();
		uint64_t sampleCount = m_Buffer.getSampleCount();

		samples.resize(sampleCount);

		uint64_t effectiveOffset = offset % sampleCount;

		std::copy(bufferSamples + effectiveOffset, bufferSamples + sampleCount, samples.begin() + effectiveOffset);
	}

} // Mix