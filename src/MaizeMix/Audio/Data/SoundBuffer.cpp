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

	const sf::SoundBuffer& SoundBuffer::GetBuffer() const
	{
		return m_Buffer;
	}

} // Mix