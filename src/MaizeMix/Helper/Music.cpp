#include "MaizeMix/Helper/Music.h"
#include "MaizeMix/Helper/AudioClips/SoundReference.h"

namespace Mix {

	Music::~Music()
	{
		sf::Music::stop();

		if (m_Reference != nullptr)
		{
			m_Reference->DetachReference(this);
		}
	}

	bool Music::setSoundReference(const SoundReference& musicBuffer)
	{
		if (m_Reference != nullptr)
		{
			sf::Music::stop();
			m_Reference->DetachReference(this);
		}

		if (openFromFile(musicBuffer.m_AudioPath))
		{
			m_Reference = &musicBuffer;
			m_Reference->AttachReference(this);

			return true;
		}

		return false;
	}

	const SoundReference* Music::getReference() const
	{
		return m_Reference;
	}

	void Music::resetReference()
	{
		sf::Music::stop();

		if (m_Reference != nullptr)
		{
			m_Reference->DetachReference(this);
			m_Reference = nullptr;
		}
	}

} // Mix