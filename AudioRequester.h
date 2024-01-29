#pragma once

#include <SFML/Audio.hpp>

#include <unordered_map>
#include <string>
#include <memory>
#include <cstdint>
#include <set>

class AudioClip;

class AudioRequester
{
 private:
	struct LoadInfo
	{
		int16_t clipID = 0;
		bool wasSuccessful = false;
	};

 public:
	static LoadInfo LoadClip(const std::string& audioPath, bool stream);
	static void UnloadClip(AudioClip& clip);

	static uint32_t GetClipChannel(const AudioClip& clip);
	static float GetClipDuration(const AudioClip& clip);
	static uint32_t GetClipFrequency(const AudioClip& clip);

 private:
	static int16_t GetID();

	inline static std::unordered_map<int16_t, std::unique_ptr<sf::SoundBuffer>> s_SoundEmitters;
	inline static std::unordered_map<int16_t, std::unique_ptr<sf::Music>> s_MusicEmitters;
	inline static std::set<int16_t> s_FreeIDs;
};