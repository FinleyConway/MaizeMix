#include "AudioEngine.h"
#include "AudioClip.h"
#include "Music.h"
#include "AudioFinishCallback.h"
#include "AudioSource.h"

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

	size_t AudioEngine::CreateAudioSource(const AudioSourceDef& audioSourceDef)
	{
		static size_t id = 0;

		id++;

		m_AudioSources.try_emplace(id, AudioSource(id, audioSourceDef, this));

		return id;
	}

	void AudioEngine::DestroyAudioSource(size_t audioSourceID)
	{
		// check if the audio source exits
		if (m_AudioSources.contains(audioSourceID))
		{
			// stop all audio that is currently playing

			// remove the audio source
			m_AudioSources.erase(audioSourceID);
		}
	}

	void AudioEngine::PlayAudio(size_t audioSourceID, float x, float y, float z)
	{
		if (HasHitMaxAudioSources()) return;

		// check if the audio source exits
		if (m_AudioSources.contains(audioSourceID))
		{
			auto& s = m_AudioSources.at(audioSourceID);
			auto shouldStream = s.m_Clip.IsLoadInBackground();

			// check if the clip is valid
			if  (s.m_Clip.m_ClipID == c_InvalidClip)
			{
				std::cerr << "Warning: Trying to play an invalid audio clip! AudioSourceID: " << (int)audioSourceID << std::endl;
				return;
			}

			// decide if you should play an audio clip from memory or stream it
			if (shouldStream)
			{
				s.m_PlayingID = PlayStreamedAudioClip(s.m_Clip, s.m_Volume, s.m_Pitch, s.m_Loop, x, y, z, s.m_MinDistance, s.m_MaxDistance);
			}
			else
			{
				s.m_PlayingID = PlayAudioClip(s.m_Clip, s.m_Volume, s.m_Pitch, s.m_Loop, x, y, z, s.m_MinDistance, s.m_MaxDistance);
			}
		}
	}

    void AudioEngine::PauseAudio(uint8_t audioSourceID)
    {
        if (m_AudioSources.contains(audioSourceID))
        {
            auto &soundData = m_CurrentPlayingSounds.at(m_AudioSources.at(audioSourceID).m_PlayingID);

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
			const auto index = m_AudioEventQueue.begin()->playingID;

			if (m_Callback != nullptr)
			{
				//m_Callback->OnAudioFinish(m_CurrentPlayingSounds.at(index));
			}

			m_CurrentPlayingSounds.erase(index);
			m_AudioEventQueue.erase(m_AudioEventQueue.begin());
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

	uint8_t AudioEngine::PlayAudioClip(AudioClip& clip, float volume, float pitch, bool loop, float x, float y, float depth, float minDistance, float maxDistance)
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
			const float stopTime = loop ? std::numeric_limits<float>::infinity() : m_CurrentTime.asSeconds() + clip.GetDuration();
			const uint8_t playingID = GetNextID();

			m_CurrentPlayingSounds.try_emplace(playingID, sound);
			m_AudioEventQueue.emplace(playingID, stopTime);

			// play the audio source
			std::get<sf::Sound>(m_CurrentPlayingSounds.at(playingID).sound).play();
		}

		return c_InvalidAudioSource;
	}

	uint8_t AudioEngine::PlayStreamedAudioClip(AudioClip& clip, float volume, float pitch, bool loop, float x, float y, float depth, float minDistance, float maxDistance)
	{
		if (m_SoundReferences.contains(clip.m_ClipID))
		{
			const auto& musicBuffer = m_SoundReferences.at(clip.m_ClipID);

			// load music source
			auto music = std::make_shared<Music>();
			music->Load(musicBuffer);

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
			const float stopTime = loop ? std::numeric_limits<float>::infinity() : m_CurrentTime.asSeconds() + clip.GetDuration();
			const uint8_t playingID = GetNextID();

			m_CurrentPlayingSounds.try_emplace(playingID, music);
			m_AudioEventQueue.emplace(playingID, stopTime);

			// play the audio source
			std::get<std::shared_ptr<Music>>(m_CurrentPlayingSounds.at(playingID).sound)->Play();
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

	void AudioEngine::ReturnID(uint8_t playingID)
	{
		m_UnusedIDs.push_back(playingID);
	}

}