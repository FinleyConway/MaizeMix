#include "AudioEngine.h"
#include "AudioClip.h"
#include "Music.h"
#include "AudioFinishCallback.h"

#include <iostream>
#include <cmath>
#include <cassert>

namespace Maize::Mix {

	AudioClip AudioEngine::CreateClip(const std::string& audioPath, bool stream)
	{
		static size_t id = 0;

		// sfml boilerplate to create audio data
		if (stream)
		{
			SoundReference soundReference;

			if (soundReference.OpenFromFile(audioPath))
			{
				id++;

				m_SoundReferences.emplace(id, soundReference);

				return { id, soundReference.GetChannelCount(),
						 soundReference.GetDuration().asSeconds(),
						 soundReference.GetSampleRate(), stream, AudioClip::LoadState::Loaded };
			}
		}
		else
		{
			sf::SoundBuffer soundBuffer;

			if (soundBuffer.loadFromFile(audioPath))
			{
				id++;

				m_SoundBuffers.emplace(id, soundBuffer);

				return { id, soundBuffer.getChannelCount(),
						 soundBuffer.getDuration().asSeconds(),
						 soundBuffer.getSampleRate(), stream, AudioClip::LoadState::Loaded };
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
			m_SoundReferences.erase(clip.m_ClipID);
		}
		else
		{
			m_SoundBuffers.erase(clip.m_ClipID);
		}

		// set clip to default
		clip = AudioClip();
	}

	uint8_t AudioEngine::PlayAudio(AudioClip& clip, float volume, float pitch, bool loop, const std::any& userData, float x, float y, float depth, float minDistance, float maxDistance)
	{
		if (clip.m_ClipID == c_InvalidClip) return c_InvalidAudioSource;

		if (HasHitMaxAudioSources())
		{
			return c_InvalidAudioSource;
		}

		if (clip.IsLoadInBackground())
		{
			return PlayStreamedAudioClip(clip, volume, pitch, loop, x, y, depth, minDistance, maxDistance, userData);
		}
		else
		{
			return PlayAudioClip(clip, volume, pitch, loop, x, y, depth, minDistance, maxDistance, userData);
		}
	}

	void AudioEngine::PauseAudio(uint8_t audioSourceID)
	{
		if (m_CurrentPlayingSounds.contains(audioSourceID))
		{
			auto &soundData = m_CurrentPlayingSounds.at(audioSourceID);

			if (auto* music = std::get_if<std::shared_ptr<Music>>(&soundData.sound))
			{
				if ((*music)->GetStatus() != sf::SoundSource::Playing) return;
				(*music)->Pause();
			}
			else if (auto* sound = std::get_if<sf::Sound>(&soundData.sound))
			{
				if (sound->getStatus() != sf::SoundSource::Playing) return;
				sound->pause();
			}

			assert(soundData.event != nullptr);
			m_AudioEventQueue.erase(*soundData.event);
		}
	}

	void AudioEngine::UnpauseAudio(uint8_t audioSourceID)
	{
		if (m_CurrentPlayingSounds.contains(audioSourceID))
		{
			auto& soundData = m_CurrentPlayingSounds.at(audioSourceID);

			if (auto* music = std::get_if<std::shared_ptr<Music>>(&soundData.sound))
			{
				if ((*music)->GetStatus() != sf::SoundSource::Paused) return;
				(*music)->Play();
			}
			else if (auto* sound = std::get_if<sf::Sound>(&soundData.sound))
			{
				if (sound->getStatus() != sf::SoundSource::Paused) return;
				sound->play();
			}

			float stopTime = m_CurrentTime.asSeconds() + GetPlayingOffset(soundData.sound);

			m_AudioEventQueue.emplace(audioSourceID, stopTime);
		}
	}

	void AudioEngine::StopAudio(uint8_t audioSourceID)
	{
		if (m_CurrentPlayingSounds.contains(audioSourceID))
		{
			auto& soundData = m_CurrentPlayingSounds.at(audioSourceID);

			if (auto* music = std::get_if<std::shared_ptr<Music>>(&soundData.sound))
			{
				(*music)->Stop();
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
	}

	void AudioEngine::SetAudioLoopState(uint8_t audioSourceID, bool loop)
	{
		if (m_CurrentPlayingSounds.contains(audioSourceID))
		{
			auto& soundData = m_CurrentPlayingSounds.at(audioSourceID);

			if (auto* music = std::get_if<std::shared_ptr<Music>>(&soundData.sound))
			{
				if ((*music)->GetLoop() == loop) return;
				(*music)->SetLoop(loop);
			}
			else if (auto* sound = std::get_if<sf::Sound>(&soundData.sound))
			{
				if (sound->getLoop() == loop) return;
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

	void AudioEngine::SetAudioMuteState(uint8_t audioSourceID, bool mute)
	{
		if (m_CurrentPlayingSounds.contains(audioSourceID))
		{
			auto &soundData = m_CurrentPlayingSounds.at(audioSourceID);

			float volume = mute ? 0.0f : soundData.previousVolume;

			if (auto* music = std::get_if<std::shared_ptr<Music>>(&soundData.sound))
			{
				soundData.previousVolume = (*music)->GetVolume();
				(*music)->SetVolume(volume);
			}
			else if (auto* sound = std::get_if<sf::Sound>(&soundData.sound))
			{
				soundData.previousVolume = sound->getVolume();
				sound->setVolume(volume);
			}
		}
	}

	void AudioEngine::SetAudioVolume(uint8_t audioSourceID, float volume)
	{
		if (m_CurrentPlayingSounds.contains(audioSourceID))
		{
			auto& soundData = m_CurrentPlayingSounds.at(audioSourceID);

			if (auto* music = std::get_if<std::shared_ptr<Music>>(&soundData.sound))
			{
				(*music)->SetVolume(LimitVolume(volume));
			}
			else if (auto* sound = std::get_if<sf::Sound>(&soundData.sound))
			{
				sound->setVolume(LimitVolume(volume));
			}
		}
	}

	void AudioEngine::SetAudioPitch(uint8_t audioSourceID, float pitch)
	{
		if (m_CurrentPlayingSounds.contains(audioSourceID))
		{
			auto& soundData = m_CurrentPlayingSounds.at(audioSourceID);

			if (auto* music = std::get_if<std::shared_ptr<Music>>(&soundData.sound))
			{
				(*music)->SetPitch(pitch);
			}
			else if (auto* sound = std::get_if<sf::Sound>(&soundData.sound))
			{
				sound->setPitch(pitch);
			}
		}
	}

	void AudioEngine::SetAudioPosition(uint8_t audioSourceID, float x, float y, float depth, float minDistance, float maxDistance)
	{
		if (m_CurrentPlayingSounds.contains(audioSourceID))
		{
			auto& soundData = m_CurrentPlayingSounds.at(audioSourceID);

			if (auto* music = std::get_if<std::shared_ptr<Music>>(&soundData.sound))
			{
				(*music)->SetPosition(x, y, depth);
				(*music)->SetMinDistance(minDistance);
				(*music)->SetMaxDistance(maxDistance);
			}
			else if (auto* sound = std::get_if<sf::Sound>(&soundData.sound))
			{
				sound->setPosition(x, y, depth);
				sound->setMinDistance(minDistance);
				sound->setAttenuation(maxDistance);
			}
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

	void AudioEngine::SetAudioFinishCallback(AudioFinishCallback* callback)
	{
		m_Callback = callback;
	}

	void AudioEngine::Update(float deltaTime)
	{
		// update audio system time
		m_CurrentTime += sf::seconds(deltaTime);

		// remove all finished sounds
		while (!m_AudioEventQueue.empty() && m_CurrentTime.asSeconds() >= m_AudioEventQueue.begin()->stopTime)
		{
			auto audioSourceID = m_AudioEventQueue.begin()->audioSourceID;

			if (m_Callback != nullptr)
			{
				m_Callback->OnAudioFinish(audioSourceID, m_CurrentPlayingSounds.at(audioSourceID).userData);
			}

			m_CurrentPlayingSounds.erase(audioSourceID);
			m_AudioEventQueue.erase(m_AudioEventQueue.begin());

			ReturnID(audioSourceID);
		}
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

	float AudioEngine::GetPlayingOffset(const std::variant<sf::Sound, std::shared_ptr<Music>>& soundVariant)
	{
		if (auto* music = std::get_if<std::shared_ptr<Music>>(&soundVariant))
		{
			return (*music)->GetPlayingOffset().asSeconds();
		}
		else if (auto* sound = std::get_if<sf::Sound>(&soundVariant))
		{
			return sound->getPlayingOffset().asSeconds();
		}

		return 0.0f;
	}

	uint8_t AudioEngine::PlayAudioClip(AudioClip& clip, float volume, float pitch, bool loop, float x, float y, float depth, float minDistance, float maxDistance, const std::any& userData)
	{
		if (m_SoundBuffers.contains(clip.m_ClipID))
		{
			const auto& buffer = m_SoundBuffers.at(clip.m_ClipID);

			// set source properties
			sf::Sound sound;
			sound.setBuffer(buffer);
			sound.setVolume(LimitVolume(volume));
			sound.setPitch(pitch);
			sound.setLoop(loop);

			if (x != 0.0f || y != 0.0f || depth != 0.0f)
			{
				sound.setPosition(x, y, depth);
				sound.setMinDistance(minDistance);
				sound.setAttenuation(maxDistance);
			}

			// get stop event time
			float stopTime = loop ? std::numeric_limits<float>::infinity() : m_CurrentTime.asSeconds() + clip.GetDuration();

			// get id
			const uint8_t audioSourceID = GetNextID();

			const auto& event = m_AudioEventQueue.emplace(audioSourceID, stopTime);
			m_CurrentPlayingSounds.try_emplace(audioSourceID, sound, &(*event.first), userData);

			// play the audio source
			std::get<sf::Sound>(m_CurrentPlayingSounds.at(audioSourceID).sound).play();

			return audioSourceID;
		}

		return c_InvalidAudioSource;
	}

	uint8_t AudioEngine::PlayStreamedAudioClip(AudioClip& clip, float volume, float pitch, bool loop, float x, float y, float depth, float minDistance, float maxDistance, const std::any& userData)
	{
		if (m_SoundReferences.contains(clip.m_ClipID))
		{
			const auto& musicBuffer = m_SoundReferences.at(clip.m_ClipID);

			// load music source
			auto music = std::make_shared<Music>();
			if (!music->Load(musicBuffer))
			{
				return c_InvalidAudioSource;
			}

			// set source properties
			music->SetVolume(LimitVolume(volume));
			music->SetPitch(pitch);
			music->SetLoop(loop);

			if (x != 0.0f || y != 0.0f || depth != 0.0f)
			{
				music->SetPosition(x, y, depth);
				music->SetMinDistance(minDistance);
				music->SetMaxDistance(maxDistance);
			}

			// get stop event time
			float stopTime = loop ? std::numeric_limits<float>::infinity() : m_CurrentTime.asSeconds() + clip.GetDuration();

			// get id
			uint8_t audioSourceID = GetNextID();

			const auto& event = m_AudioEventQueue.emplace(audioSourceID, stopTime);
			m_CurrentPlayingSounds.try_emplace(audioSourceID, music, &(*event.first), userData);

			// play the audio source
			std::get<std::shared_ptr<Music>>(m_CurrentPlayingSounds.at(audioSourceID).sound)->Play();

			return audioSourceID;
		}

		return c_InvalidAudioSource;
	}

	uint8_t AudioEngine::GetNextID()
	{
		static uint8_t newID = 1;

		// generate a new ID if there is non have been returned
		if (m_UnusedIDs.empty())
		{
			// prevent generating an ID greater than the max audio limitation
			if (newID >= c_MaxAudioEmitters) return c_InvalidAudioSource;

			return newID++;
		}

		// recycle an ID which was returned
		uint8_t nextID = m_UnusedIDs.back();
		m_UnusedIDs.pop_back();

		return nextID;
	}

	void AudioEngine::ReturnID(uint8_t audioSourceID)
	{
		m_UnusedIDs.push_back(audioSourceID);
	}

}