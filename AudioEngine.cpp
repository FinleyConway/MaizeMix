#include "AudioEngine.h"
#include "AudioClip.h"

#include <iostream>
#include <cmath>
#include <cassert>

AudioClip AudioEngine::CreateClip(const std::string& audioPath, bool stream)
{
	static size_t id = 0;

	// sfml boilerplate to create audio data
	if (stream)
	{
		auto music = std::make_shared<sf::Music>();

		if (music->openFromFile(audioPath))
		{
			id++;

			m_Music.emplace(id, music);

			return { audioPath, id,music->getChannelCount(),
					music->getDuration().asSeconds(),
					music->getSampleRate(), stream, AudioClip::LoadState::Loaded};
		}
	}
	else
	{
		auto buffer = std::make_shared<sf::SoundBuffer>();

		if (buffer->loadFromFile(audioPath))
		{
			id++;

			m_SoundBuffers.emplace(id, buffer);

			return { audioPath, id,buffer->getChannelCount(),
					buffer->getDuration().asSeconds(),
					buffer->getSampleRate(), stream, AudioClip::LoadState::Loaded};
		}
	}

	// return a failed audio clip
	return { c_InvalidClip, stream, AudioClip::LoadState::Failed };
}

void AudioEngine::DestroyClip(AudioClip& clip)
{
	// remove clip
	if (clip.m_IsStreaming)
	{
		m_Music.erase(clip.m_ClipID);
	}
	else
	{
		m_SoundBuffers.erase(clip.m_ClipID);
	}

	// set clip to default
	clip = AudioClip();
}

uint8_t AudioEngine::PlaySound(AudioClip& clip, float volume, float pitch, bool loop)
{
	if (clip.m_ClipID == c_InvalidClip) return c_InvalidAudioSource;

	if (HasHitMaxAudioSources())
	{
		return c_InvalidAudioSource;
	}

	if (clip.IsLoadInBackground())
	{
		return PlayStreamedAudio(clip, volume, pitch, loop);
	}
	else
	{
		return PlayAudio(clip, volume, pitch, loop);
	}
}

uint8_t AudioEngine::PlaySoundAtPosition(AudioClip& clip, float volume, float pitch, bool loop, float x, float y, float depth, float minDistance, float maxDistance)
{
	if (clip.m_ClipID == c_InvalidClip) return c_InvalidAudioSource;

	if (HasHitMaxAudioSources())
	{
		return c_InvalidAudioSource;
	}

	if (clip.IsLoadInBackground())
	{
		return PlayStreamedAudio(clip, volume, pitch, loop, x, y, depth, minDistance, maxDistance);
	}
	else
	{
		return PlayAudio(clip, volume, pitch, loop, x, y, depth, minDistance, maxDistance);
	}
}

void AudioEngine::UpdateSoundLoopState(uint8_t audioSourceID, bool loop)
{
	if (m_CurrentPlayingSounds.contains(audioSourceID))
	{
		auto& soundData = m_CurrentPlayingSounds.at(audioSourceID);

		// update sound loop state
		if (auto* music = std::get_if<std::shared_ptr<sf::Music>>(&soundData.sound))
		{
			(*music)->setLoop(loop);
		}
		else if (auto* sound = std::get_if<sf::Sound>(&soundData.sound))
		{
			sound->setLoop(loop);
		}

		// remove current sound event
		assert(soundData.event != nullptr);
		m_AudioEventQueue.erase(*soundData.event);

		// recreate sound event with new event time
		float stopTime = loop ? std::numeric_limits<float>::infinity() : m_CurrentTime.asSeconds() + GetPlayingOffset(soundData.sound);

		m_AudioEventQueue.emplace(audioSourceID, stopTime);
	}
}

void AudioEngine::UpdateSoundVolume(uint8_t audioSourceID, float volume)
{
	if (m_CurrentPlayingSounds.contains(audioSourceID))
	{
		auto& soundData = m_CurrentPlayingSounds.at(audioSourceID);

		if (auto* music = std::get_if<std::shared_ptr<sf::Music>>(&soundData.sound))
		{
			(*music)->setVolume(LimitVolume(volume));
		}
		else if (auto* sound = std::get_if<sf::Sound>(&soundData.sound))
		{
			sound->setVolume(LimitVolume(volume));
		}
	}
}

void AudioEngine::UpdateSoundPitch(uint8_t audioSourceID, float pitch)
{
	if (m_CurrentPlayingSounds.contains(audioSourceID))
	{
		auto& soundData = m_CurrentPlayingSounds.at(audioSourceID);

		if (auto* music = std::get_if<std::shared_ptr<sf::Music>>(&soundData.sound))
		{
			(*music)->setPitch(pitch);
		}
		else if (auto* sound = std::get_if<sf::Sound>(&soundData.sound))
		{
			sound->setPitch(pitch);
		}
	}
}

void AudioEngine::UpdateSoundPosition(uint8_t audioSourceID, float x, float y, float depth, float minDistance, float maxDistance)
{
	if (m_CurrentPlayingSounds.contains(audioSourceID))
	{
		auto& soundData = m_CurrentPlayingSounds.at(audioSourceID);

		if (auto* music = std::get_if<std::shared_ptr<sf::Music>>(&soundData.sound))
		{
			(*music)->setPosition(x, y, depth);
			(*music)->setMinDistance(minDistance);
			(*music)->setAttenuation(maxDistance);
		}
		else if (auto* sound = std::get_if<sf::Sound>(&soundData.sound))
		{
			sound->setPosition(x, y, depth);
			sound->setMinDistance(minDistance);
			sound->setAttenuation(maxDistance);
		}
	}
}

void AudioEngine::SetAudioState(uint8_t audioSourceID, AudioState audioState)
{
	if (!m_CurrentPlayingSounds.contains(audioSourceID)) return;

	auto& soundData = m_CurrentPlayingSounds.at(audioSourceID);

	if (audioState == AudioState::Pause)
	{
		PauseSound(soundData);
	}
	else if (audioState == AudioState::Unpause)
	{
		UnpauseSound(audioSourceID, soundData);
	}
	else if (audioState == AudioState::Mute)
	{
		MuteSound(soundData);
	}
	else if (audioState == AudioState::Unmute)
	{
		UnmuteSound(soundData);
	}
	else if (audioState == AudioState::Stop)
	{
		StopSound(audioSourceID, soundData);
	}
}

void AudioEngine::Update(float deltaTime)
{
	// update audio system time
	m_CurrentTime += sf::seconds(deltaTime);

	// remove all finished sounds
	while (!m_AudioEventQueue.empty() && m_CurrentTime.asSeconds() >= m_AudioEventQueue.begin()->stopTime)
	{
		auto audioSourceID = m_AudioEventQueue.begin()->audioSourceID;

		m_CurrentPlayingSounds.erase(audioSourceID);
		m_AudioEventQueue.erase(m_AudioEventQueue.begin());

		ReturnID(audioSourceID);
	}
}

void AudioEngine::SetListenerPosition(float x, float y, float depth)
{
	if (m_CurrentPlayingSounds.empty()) return;

	sf::Listener::setPosition(x, y, depth);
}

void AudioEngine::SetGlobalVolume(float volume)
{
	if (m_CurrentPlayingSounds.empty()) return;

	sf::Listener::setGlobalVolume(LimitVolume(volume));
}

float AudioEngine::LimitVolume(float volume) const
{
	return std::clamp(volume, 0.0f, 100.0f);;
}

bool AudioEngine::HasHitMaxAudioSources() const
{
	if (m_AudioEventQueue.size() >= c_MaxAudioEmitters)
	{
		std::cerr << "Warning: Max audio reached! Cannot play more audio clips!" << std::endl;
		return true;
	}

	return false;
}

float AudioEngine::GetPlayingOffset(const std::variant<sf::Sound, std::shared_ptr<sf::Music>>& soundVariant)
{
	if (auto* music = std::get_if<std::shared_ptr<sf::Music>>(&soundVariant))
	{
		return (*music)->getPlayingOffset().asSeconds();
	}
	else if (auto* sound = std::get_if<sf::Sound>(&soundVariant))
	{
		return sound->getPlayingOffset().asSeconds();
	}

	return 0.0f;
}

void AudioEngine::PauseSound(Audio& soundData)
{
	if (auto* music = std::get_if<std::shared_ptr<sf::Music>>(&soundData.sound))
	{
		(*music)->pause();
	}
	else if (auto* sound = std::get_if<sf::Sound>(&soundData.sound))
	{
		sound->pause();
	}

	m_AudioEventQueue.erase(*soundData.event);
}

void AudioEngine::UnpauseSound(uint8_t audioSourceID, Audio& soundData)
{
	if (auto* music = std::get_if<std::shared_ptr<sf::Music>>(&soundData.sound))
	{
		(*music)->play();
	}
	else if (auto* sound = std::get_if<sf::Sound>(&soundData.sound))
	{
		sound->play();
	}

	float stopTime = m_CurrentTime.asSeconds() + GetPlayingOffset(soundData.sound);

	m_AudioEventQueue.emplace(audioSourceID, stopTime);
}

void AudioEngine::MuteSound(Audio& soundData)
{
	if (auto* music = std::get_if<std::shared_ptr<sf::Music>>(&soundData.sound))
	{
		soundData.previousVolume = (*music)->getVolume();
		(*music)->setVolume(0);
	}
	else if (auto* sound = std::get_if<sf::Sound>(&soundData.sound))
	{
		soundData.previousVolume = sound->getVolume();
		sound->setVolume(0);
	}
}

void AudioEngine::UnmuteSound(Audio& soundData)
{
	if (auto* music = std::get_if<std::shared_ptr<sf::Music>>(&soundData.sound))
	{
		(*music)->setVolume(soundData.previousVolume);
	}
	else if (auto* sound = std::get_if<sf::Sound>(&soundData.sound))
	{
		sound->setVolume(soundData.previousVolume);
	}
}

void AudioEngine::StopSound(uint8_t audioSourceID, Audio& soundData)
{
	if (auto* music = std::get_if<std::shared_ptr<sf::Music>>(&soundData.sound))
	{
		(*music)->stop();
	}
	else if (auto* sound = std::get_if<sf::Sound>(&soundData.sound))
	{
		sound->stop();
	}

	assert(soundData.event != nullptr);

	m_AudioEventQueue.erase(*soundData.event);
	m_CurrentPlayingSounds.erase(audioSourceID);

	ReturnID(audioSourceID);
}

uint8_t AudioEngine::PlayAudio(const AudioClip& clip, float volume, float pitch, bool loop, float x, float y, float depth, float minDistance, float maxDistance)
{
	const auto& buffer = m_SoundBuffers.at(clip.m_ClipID);

	if (buffer != nullptr)
	{
		sf::Sound sound;
		sound.setBuffer(*buffer);
		sound.setVolume(LimitVolume(volume));
		sound.setPitch(pitch);
		sound.setLoop(loop);

		if (x != 0.0f || y != 0.0f || depth != 0.0f)
		{
			sound.setPosition(x, y, depth);
			sound.setMinDistance(minDistance);
			sound.setAttenuation(maxDistance);
		}

		float stopTime = loop ? std::numeric_limits<float>::infinity() : m_CurrentTime.asSeconds() + clip.GetDuration();
		const uint8_t audioSourceID = GetNextID();

		const auto& event = m_AudioEventQueue.emplace(audioSourceID, stopTime);

		m_CurrentPlayingSounds.try_emplace(audioSourceID, sound, &(*event.first));
		std::get_if<sf::Sound>(&m_CurrentPlayingSounds.at(audioSourceID).sound)->play();

		return audioSourceID;
	}

	return c_InvalidAudioSource;
}

uint8_t AudioEngine::PlayStreamedAudio(AudioClip& clip, float volume, float pitch, bool loop, float x, float y, float depth, float minDistance, float maxDistance)
{
	const auto& musicBuffer = m_Music.at(clip.m_ClipID);

	if (musicBuffer != nullptr)
	{
		std::shared_ptr<sf::Music> music;

		if (clip.m_RefCounter > 0)
		{
			music = std::make_shared<sf::Music>();

			if (!music->openFromFile(clip.m_FilePath))
			{
				return c_InvalidAudioSource;
			}
		}
		else
		{
			music = musicBuffer;
		}

		clip.m_RefCounter++;

		music->setVolume(LimitVolume(volume));
		music->setPitch(pitch);
		music->setLoop(loop);

		if (x != 0.0f || y != 0.0f || depth != 0.0f)
		{
			music->setPosition(x, y, depth);
			music->setMinDistance(minDistance);
			music->setAttenuation(maxDistance);
		}

		float stopTime = loop ? std::numeric_limits<float>::infinity() : m_CurrentTime.asSeconds() + clip.GetDuration();
		uint8_t audioSourceID = GetNextID();

		const auto& event = m_AudioEventQueue.emplace(audioSourceID, stopTime);

		m_CurrentPlayingSounds.try_emplace(audioSourceID, music, &(*event.first));
		(*std::get_if<std::shared_ptr<sf::Music>>(&m_CurrentPlayingSounds.at(audioSourceID).sound))->play();

		return audioSourceID;
	}

	return c_InvalidAudioSource;
}
