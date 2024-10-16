#pragma once

#include <SFML/Audio.hpp>
#include <cstdint>

namespace Mix {

	class Clip
	{
	 public:
		virtual ~Clip() = default;

		virtual bool OpenFromFile(const std::string& filename) = 0;

		virtual sf::Time GetDuration() const = 0;
		virtual uint32_t GetChannelCount() const = 0;
		virtual uint32_t GetSampleRate() const = 0;
		virtual uint64_t GetSampleCount() const = 0;
	};

} // Mix