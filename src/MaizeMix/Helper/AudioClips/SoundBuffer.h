#pragma once

#include <SFML/Audio.hpp>

#include "MaizeMix/Helper/AudioClips/Clip.h"

namespace Mix {

 	class SoundBuffer final : public Clip
	{
	 public:
		bool OpenFromFile(const std::string& filename) override;

		sf::Time GetDuration() const override;
		uint32_t GetChannelCount() const override;
		uint32_t GetSampleRate() const override;
		uint64_t GetSampleCount() const override;

		const sf::SoundBuffer& GetBuffer() const;

	private:
		sf::SoundBuffer m_Buffer;
	};

} // Mix