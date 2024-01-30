#include "AudioEngine.h"
#include "AudioRequester.h"
#include "AudioClip.h"

void AudioEngine::PlaySound(const AudioClip& clip, float volume, float pitch, bool loop)
{
	const auto& buffer = AudioRequester::GetClipData(clip);

	if (buffer != nullptr)
	{
		sf::Sound sound;
		sound.setBuffer(*buffer);
		sound.setVolume(volume);
		sound.setPitch(pitch);
		sound.setLoop(loop);

		float stopTime;

		if (loop)
		{
			stopTime = std::numeric_limits<float>::infinity();
		}
		else
		{
			stopTime = clip.GetDuration();
		}

		m_SoundEventQueue.emplace(clip.m_ClipID, stopTime);
		m_CurrentPlayingSounds.try_emplace(clip.m_ClipID, sound, buffer);

		m_CurrentPlayingSounds[clip.m_ClipID].sound.play();
	}
}

void AudioEngine::StopSound(const AudioClip& clip)
{
	if (m_CurrentPlayingSounds.contains(clip.m_ClipID))
	{
		auto& soundData = m_CurrentPlayingSounds.at(clip.m_ClipID);

		soundData.sound.stop();

		//m_SoundEventQueue.erase(); need to find a way to remove this efficiently
		m_CurrentPlayingSounds.erase(clip.m_ClipID);
	}
}

void AudioEngine::Update(float currentTime)
{
	// remove all finished sounds
	while (!m_SoundEventQueue.empty() && currentTime >= m_SoundEventQueue.begin()->stopTime)
	{
		m_SoundEventQueue.erase(m_SoundEventQueue.begin());
		m_CurrentPlayingSounds.erase(m_SoundEventQueue.begin()->clipID);
	}
}
