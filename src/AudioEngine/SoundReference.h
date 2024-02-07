#pragma once

#include "SFML/Audio.hpp"
#include <cstdint>
#include <string>

class Music;

class SoundReference
{
 public:
	SoundReference() = default;
	~SoundReference();

	bool OpenFromFile(const std::string& filename);

	sf::Time GetDuration() const;
	uint32_t GetChannelCount() const;
	uint32_t GetSampleRate() const;

 private:
	void AttachReference(Music* music) const;
	void DetachReference(Music* music) const;

 private:
	friend class Music;

	sf::Time m_Duration;
	uint32_t m_ChannelCount = 0;
	uint32_t m_SampleRate = 0;

	std::string m_AudioPath; // used for sf::Music to act like setBuffer which would instead do openFromFile
	mutable std::set<Music*> m_References;
};
