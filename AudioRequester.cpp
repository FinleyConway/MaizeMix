#include "AudioRequester.h"
#include "AudioClip.h"

AudioRequester::LoadInfo AudioRequester::LoadClip(const std::string& audioPath, bool stream)
{
	// handle audio that will be streamed
	if (stream)
	{
		auto music = std::make_unique<sf::Music>();

		if (music->openFromFile(audioPath))
		{
			const auto id = GetID();

			s_MusicEmitters.emplace(id, std::move(music));
			return LoadInfo(id, true);
		}
	}
		// handle audio that will be loaded into memory
	else
	{
		auto buffer = std::make_unique<sf::SoundBuffer>();

		if (buffer->loadFromFile(audioPath))
		{
			const auto id = GetID();

			s_SoundEmitters.emplace(id, std::move(buffer));
			return LoadInfo(id, true);
		}
	}

	return LoadInfo(0, false);
}

void AudioRequester::UnloadClip(AudioClip& clip)
{
	// return due to invalid audio clip
	if (clip.m_ClipID == -1) return;

	s_FreeIDs.emplace(clip.m_ClipID);

	// unload clip
	if (clip.m_IsStreaming)
	{
		s_MusicEmitters.erase(clip.m_ClipID);
		clip.m_LoadState = AudioClip::LoadState::Unloaded;
		clip.m_ClipID = -1;
	}
	else
	{
		s_SoundEmitters.erase(clip.m_ClipID);
		clip.m_LoadState = AudioClip::LoadState::Unloaded;
		clip.m_ClipID = -1;
	}
}

uint32_t AudioRequester::GetClipChannel(const AudioClip& clip)
{
	if (clip.m_ClipID == -1) return 0;

	if (clip.m_IsStreaming)
	{
		const auto& music = s_MusicEmitters.at(clip.m_ClipID);
		return music->getChannelCount();
	}
	else
	{
		const auto& sound = s_SoundEmitters.at(clip.m_ClipID);
		return sound->getChannelCount();
	}
}

float AudioRequester::GetClipDuration(const AudioClip& clip)
{
	if (clip.m_ClipID == -1) return 0;

	if (clip.m_IsStreaming)
	{
		const auto& music = s_MusicEmitters.at(clip.m_ClipID);
		return music->getDuration().asSeconds();
	}
	else
	{
		const auto& sound = s_SoundEmitters.at(clip.m_ClipID);
		return sound->getDuration().asSeconds();
	}
}

uint32_t AudioRequester::GetClipFrequency(const AudioClip& clip)
{
	if (clip.m_ClipID == -1) return 0;

	if (clip.m_IsStreaming)
	{
		const auto& music = s_MusicEmitters.at(clip.m_ClipID);
		return music->getSampleRate();
	}
	else
	{
		const auto& sound = s_SoundEmitters.at(clip.m_ClipID);
		return sound->getSampleRate();
	}
}

int16_t AudioRequester::GetID()
{
	static int16_t nextID = 0;

	if (s_FreeIDs.empty())
	{
		return nextID++;
	}
	else
	{
		const auto newID = *s_FreeIDs.begin();
		s_FreeIDs.erase(s_FreeIDs.begin());

		return newID;
	}
}
