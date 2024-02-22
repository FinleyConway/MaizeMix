#pragma once

#include <SFML/Audio.hpp>
#include "Clip.h"

namespace Mix {

 	class SoundBuffer : public Clip
	{
	 public:
		bool OpenFromFile(const std::string& filename) override;

		sf::Time GetDuration() const override;
		uint32_t GetChannelCount() const override;
		uint32_t GetSampleRate() const override;
		uint64_t GetSampleCount() const override;

		void GetData(std::vector<int16_t>& samples, uint64_t offset) override;

		const sf::SoundBuffer& GetBuffer() const { return m_Buffer; }

	 private:
		sf::SoundBuffer m_Buffer;
	};

} // Mix