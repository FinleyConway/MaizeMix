#pragma once

#include <SFML/Audio.hpp>
#include <cstdint>
#include <string>

#include "MaizeMix/Helper/AudioClips/Clip.h"

namespace Mix {

	class Music;

	class SoundReference final : public Clip
	{
	 public:
		SoundReference() = default;
		~SoundReference() override;

		bool OpenFromFile(const std::string& filename) override;

		sf::Time GetDuration() const override;
		uint32_t GetChannelCount() const override;
		uint32_t GetSampleRate() const override;
		uint64_t GetSampleCount() const override;

	 private:
		void AttachReference(Music* music) const;
		void DetachReference(Music* music) const;

	 private:
		friend class Music;

		sf::Time m_Duration;
		uint32_t m_ChannelCount = 0;
		uint32_t m_SampleRate = 0;
		uint64_t m_SampleCount = 0;

		sf::InputSoundFile m_File;
		std::string m_AudioPath; // used for sf::Music to act like setBuffer which would instead do openFromFile
		mutable std::set<Music*> m_References;
	};

} // Mix