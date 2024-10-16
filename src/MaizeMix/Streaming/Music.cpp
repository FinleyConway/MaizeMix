#include "MaizeMix/Streaming/Music.h"
#include "MaizeMix/Streaming/SoundReference.h"

namespace Mix {

	Music::Music(const SoundReference& soundReference)
	{
		Load(soundReference);
	}

	Music::~Music()
	{
		sf::Music::stop();

		if (m_Reference != nullptr)
		{
			m_Reference->DetachReference(this);
		}
	}

	void Music::Play()
	{
		sf::Music::play();
	}

	void Music::Pause()
	{
		sf::Music::pause();
	}

	void Music::Stop()
	{
		sf::Music::stop();
	}

	bool Music::Load(const SoundReference& musicBuffer)
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

	void Music::SetVolume(float volume)
	{
		sf::Music::setVolume(volume);
	}

	void Music::SetPitch(float pitch)
	{
		sf::Music::setPitch(pitch);
	}

	void Music::SetLoop(bool loop)
	{
		sf::Music::setLoop(loop);
	}

	void Music::SetPosition(float x, float y, float depth)
	{
		sf::Music::setPosition(x, y, depth);
	}

	void Music::SetMinDistance(float distance)
	{
		sf::Music::setMinDistance(distance);
	}

	void Music::SetMaxDistance(float distance)
	{
		sf::Music::setAttenuation(distance);
	}

	void Music::SetPlayingOffset(float seconds)
	{
		sf::Music::setPlayingOffset(sf::seconds(seconds));
	}

	void Music::SetRelativeToListener(bool relative)
	{
		sf::Music::setRelativeToListener(relative);
	}

	const SoundReference* Music::GetReference() const
	{
		return m_Reference;
	}

	float Music::GetVolume() const
	{
		return sf::Music::getVolume();
	}

	bool Music::GetLoop() const
	{
		return sf::Music::getLoop();
	}

	sf::Time Music::GetPlayingOffset() const
	{
		return sf::Music::getPlayingOffset();
	}

	sf::Time Music::GetDuration() const
	{
		return sf::Music::getDuration();
	}

	sf::SoundSource::Status Music::GetStatus() const
	{
		return sf::Music::getStatus();
	}

	bool Music::IsRelativeToListener() const
	{
		return sf::Music::isRelativeToListener();
	}

	void Music::ResetReference()
	{
		sf::Music::stop();

		if (m_Reference != nullptr)
		{
			m_Reference->DetachReference(this);
			m_Reference = nullptr;
		}
	}

} // Mix