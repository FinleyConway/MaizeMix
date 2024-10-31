#pragma once

#include <SFML/Audio.hpp>

namespace Mix {

	class SoundReference;

	/**
	 * Simple wrapper of sf::Music to allow it to act as sf::Sound
	 * Still acts like sf::Music but stops if the audio clip (SoundReference) goes out of scope
	 */
	class Music final : public sf::Music
	{
	 public:
		Music() = default;
		~Music() override;

		bool setSoundReference(const SoundReference& musicBuffer);
		const SoundReference* getReference() const;
		void resetReference();

	 private:
		const SoundReference* m_Reference = nullptr;
	};

} // Mix