#pragma once

#include <string>
#include <cstdint>

class AudioClip
{
 private:
	enum class LoadState { Unloaded = 0, Loaded, Failed };

 public:
	AudioClip() = default;
	~AudioClip() = default;

	uint32_t GetChannels() const { return m_Channels; }
	float GetDuration() const { return m_Duration; }
	uint32_t GetFrequency() const { return m_Frequency; }
	bool IsLoadInBackground() const { return m_IsStreaming; }
	LoadState GetLoadState() const { return m_LoadState; }

	bool operator==(const AudioClip& other) const;

 private:
	friend class AudioEngine;

	AudioClip(int16_t clipID, uint32_t channels, float duration, uint32_t frequency, bool stream, LoadState loadState);
	AudioClip(int16_t clipID, bool stream, LoadState loadState);

	int16_t m_ClipID = -1;
	LoadState m_LoadState = LoadState::Unloaded;

	uint32_t m_Channels = 0;
	float m_Duration = 0;
	uint32_t m_Frequency = 0;
	bool m_IsStreaming = false;
};
