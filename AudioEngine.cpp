#include "AudioEngine.h"
#include "AudioRequester.h"
#include "AudioClip.h"

void AudioEngine::PlaySound(const AudioClip& clip, float volume, float pitch, bool loop)
{
    if (GetCurrentAudioCount() >= c_MaxAudioEmitters)
    {
        std::cerr << "Warning: Max audio reached!" << std::endl;
    }

	const auto& buffer = AudioRequester::GetClipData(clip);

	if (buffer != nullptr)
	{
		sf::Sound sound;
		sound.setBuffer(*buffer);
		sound.setVolume(LimitVolume(volume));
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

		const auto& event = m_SoundEventQueue.emplace(clip.m_ClipID, stopTime);
		m_CurrentPlayingSounds.try_emplace(clip.m_ClipID, sound, buffer, &(*event.first));

		m_CurrentPlayingSounds[clip.m_ClipID].sound.play();
	}
}

void AudioEngine::PlaySoundAtPosition(const AudioClip &clip, float volume, float pitch, bool loop, float x, float y, float depth, float minDistance, float maxDistance)
{
    if (GetCurrentAudioCount() >= c_MaxAudioEmitters)
    {
        std::cerr << "Warning: Max audio reached!" << std::endl;
    }

    const auto& buffer = AudioRequester::GetClipData(clip);

    if (buffer != nullptr)
    {
        sf::Sound sound;
        sound.setBuffer(*buffer);
        sound.setVolume(LimitVolume(volume));
        sound.setPitch(pitch);
        sound.setLoop(loop);
        sound.setPosition(x, y, depth);
        sound.setMinDistance(minDistance);
        sound.setAttenuation(maxDistance);

        float stopTime;

        if (loop)
        {
            stopTime = std::numeric_limits<float>::infinity();
        }
        else
        {
            stopTime = clip.GetDuration();
        }

        const auto& event = m_SoundEventQueue.emplace(clip.m_ClipID, stopTime);
        m_CurrentPlayingSounds.try_emplace(clip.m_ClipID, sound, buffer, &(*event.first));

        m_CurrentPlayingSounds[clip.m_ClipID].sound.play();
    }
}

void AudioEngine::PauseSound(const AudioClip& clip)
{
    if (m_CurrentPlayingSounds.contains(clip.m_ClipID))
    {
        auto& soundData = m_CurrentPlayingSounds.at(clip.m_ClipID);

        soundData.sound.pause();

        m_SoundEventQueue.erase(*soundData.event);
    }
}

void AudioEngine::MuteSound(const AudioClip &clip)
{
    if (m_CurrentPlayingSounds.contains(clip.m_ClipID))
    {
        auto& soundData = m_CurrentPlayingSounds.at(clip.m_ClipID);

        soundData.previousVolume = soundData.sound.getVolume();
        soundData.sound.setVolume(0);
    }
}

void AudioEngine::UnMuteSound(const AudioClip &clip)
{
    if (m_CurrentPlayingSounds.contains(clip.m_ClipID))
    {
        auto& soundData = m_CurrentPlayingSounds.at(clip.m_ClipID);

        soundData.sound.setVolume(soundData.previousVolume);
    }
}

void AudioEngine::StopSound(const AudioClip& clip)
{
	if (m_CurrentPlayingSounds.contains(clip.m_ClipID))
	{
		auto& soundData = m_CurrentPlayingSounds.at(clip.m_ClipID);

		soundData.sound.stop();

        if (soundData.event != nullptr)
		    m_SoundEventQueue.erase(*soundData.event);
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
