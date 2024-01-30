#pragma once

#include <string>
#include <cstdint>

class AudioClip
{
 private:
	enum class LoadState { Unloaded = 0, Loaded, Failed };

 public:
	AudioClip() = default;
	explicit AudioClip(const std::string& audioPath, bool stream);
	~AudioClip();

	uint32_t GetChannels() const;
	float GetDuration() const;
	uint32_t GetFrequency() const;
	bool IsLoadInBackground() const;
	LoadState GetLoadState() const;

	bool operator==(const AudioClip& other) const;

 private:
	friend class AudioRequester;
	friend class AudioEngine;

	int16_t m_ClipID = -1;
	LoadState m_LoadState = LoadState::Unloaded;
	bool m_IsStreaming = false;
};
