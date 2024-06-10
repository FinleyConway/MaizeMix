#include "MaizeMix/Engine/AudioEngine.h"
#include "MaizeMix/Engine/AudioFinishCallback.h"

#include "MaizeMix/Audio/Data/SoundReference.h"
#include "MaizeMix/Audio/Data/SoundBuffer.h"
#include "MaizeMix/Audio/AudioClip.h"
#include "MaizeMix/Audio/Music.h"

#include <iostream>
#include <cmath>
#include <cassert>

namespace Mix {

	uint8_t AudioEngine::PlayAudio(AudioClip& clip, float volume, float pitch, bool loop, const std::any& userData, float x, float y, float depth, float minDistance, float maxDistance)
	{
		if (!clip.IsValid() || HasHitMaxAudioSources()) return c_InvalidAudioSource;

		if (clip.IsLoadInBackground())
		{
			return PlayStreamedAudioClip(clip, volume, pitch, loop, x, y, depth, minDistance, maxDistance, userData);
		}
		else
		{
			return PlayAudioClip(clip, volume, pitch, loop, x, y, depth, minDistance, maxDistance, userData);
		}
	}

	void AudioEngine::PauseAudio(uint8_t playingID)
	{
        // check if the requested audio is playing
		if (m_CurrentPlayingAudio.contains(playingID))
		{
			auto& soundData = m_CurrentPlayingAudio.at(playingID);

            // check to see if it's either a music clip or sound effect clip
            if (soundData.IsSoundValid())
            {
                // pause the clip if its playing
                if (soundData.sound.getStatus() != sf::SoundSource::Playing)
                {
                    soundData.sound.pause();
                }
            }
            else if (soundData.IsMusicValid())
            {
                // pause the clip if its playing
                if (soundData.music->GetStatus() != sf::SoundSource::Playing)
                {
                    soundData.music->Pause();
                }
            }

            // this shouldn't really happen, but ill place this here just in-case...
			assert(soundData.event != nullptr);

            // remove clip from the audio queue
			m_AudioEventQueue.erase(*soundData.event);
		}
	}

	void AudioEngine::UnpauseAudio(uint8_t playingID)
	{
        // check if the requested audio is playing
		if (m_CurrentPlayingAudio.contains(playingID))
		{
			auto& soundData = m_CurrentPlayingAudio.at(playingID);

            // check to see if it's either a music clip or sound effect clip
            if (soundData.IsSoundValid())
            {
                // pause the clip if its playing
                if (soundData.sound.getStatus() != sf::SoundSource::Paused)
                {
                    soundData.sound.play();

                    // add the audio clip back into the queue based on its playing position
                    float playingTimeLeft = GetDuration(soundData.sound) - GetPlayingOffset(soundData.sound);
                    float stopTime = m_CurrentTime.asSeconds() + playingTimeLeft;

                    m_AudioEventQueue.emplace(playingID, stopTime);
                }
            }
            else if (soundData.IsMusicValid())
            {
                // pause the clip if its playing
                if (soundData.music->GetStatus() != sf::SoundSource::Paused)
                {
                    soundData.music->Play();

                    // add the audio clip back into the queue based on its playing position
                    float playingTimeLeft = GetDuration(soundData.music) - GetPlayingOffset(soundData.music);
                    float stopTime = m_CurrentTime.asSeconds() + playingTimeLeft;

                    m_AudioEventQueue.emplace(playingID, stopTime);
                }
            }
		}
	}

	void AudioEngine::StopAudio(uint8_t playingID)
	{
        // check if the requested audio is playing
		if (m_CurrentPlayingAudio.contains(playingID))
		{
			auto& soundData = m_CurrentPlayingAudio.at(playingID);

            // check to see if it's either a music clip or sound effect clip
            if (soundData.IsSoundValid())
            {
                soundData.sound.stop();
            }
            else if (soundData.IsMusicValid())
            {
                soundData.music->Stop();
            }

            // this shouldn't really happen, but ill place this here just in-case...
			assert(soundData.event != nullptr);

            // trigger event handle any outside finished logic
			m_Callback->OnAudioFinish(playingID, soundData.userData);

            // remove the audio from engine
			m_AudioEventQueue.erase(*soundData.event);
			m_CurrentPlayingAudio.erase(playingID);

            // give back the playing id
			ReturnID(playingID);
		}
	}

	void AudioEngine::SetAudioLoopState(uint8_t playingID, bool loop)
	{
        // check if the requested audio is playing
		if (m_CurrentPlayingAudio.contains(playingID))
		{
			auto& soundData = m_CurrentPlayingAudio.at(playingID);

            // check to see if it's either a music clip or sound effect clip
            if (soundData.IsSoundValid())
            {
                // leave function if the same state
                if (soundData.sound.getLoop() == loop) return;

                 soundData.sound.setLoop(loop);

                // this shouldn't really happen, but ill place this here just in-case...
                assert(soundData.event != nullptr);

                // remove current sound event
                m_AudioEventQueue.erase(*soundData.event);

                // recreate sound event with new event time
                float playingTimeLeft = GetDuration(soundData.sound) - GetPlayingOffset(soundData.sound);
                float stopTime = loop ? std::numeric_limits<float>::max() : m_CurrentTime.asSeconds() + playingTimeLeft;

                m_AudioEventQueue.emplace(playingID, stopTime);
            }
            else if (soundData.IsMusicValid())
            {
                // leave function if the same state
                if (soundData.music->GetLoop() == loop) return;

                soundData.music->SetLoop(loop);

                // this shouldn't really happen, but ill place this here just in-case...
                assert(soundData.event != nullptr);

                // remove current sound event
                m_AudioEventQueue.erase(*soundData.event);

                // recreate sound event with new event time
                float playingTimeLeft = GetDuration(soundData.music) - GetPlayingOffset(soundData.music);
                float stopTime = loop ? std::numeric_limits<float>::max() : m_CurrentTime.asSeconds() + playingTimeLeft;

                m_AudioEventQueue.emplace(playingID, stopTime);
            }
		}
	}

	void AudioEngine::SetAudioMuteState(uint8_t playingID, bool mute)
	{
        // check if the requested audio is playing
		if (m_CurrentPlayingAudio.contains(playingID))
		{
			auto& soundData = m_CurrentPlayingAudio.at(playingID);
			float volume = mute ? 0.0f : soundData.previousVolume;

            soundData.isMute = mute;

            // check to see if it's either a music clip or sound effect clip
            if (soundData.IsSoundValid())
            {
                soundData.previousVolume = soundData.sound.getVolume();
                soundData.sound.setVolume(volume);
            }
            else if (soundData.IsMusicValid())
            {
                soundData.previousVolume = soundData.music->GetVolume();
                soundData.music->SetVolume(volume);
            }
		}
	}

	void AudioEngine::SetAudioVolume(uint8_t playingID, float volume)
	{
        // check if the requested audio is playing
		if (m_CurrentPlayingAudio.contains(playingID))
		{
			auto& soundData = m_CurrentPlayingAudio.at(playingID);

            if (soundData.isMute) return;

            float clampedVolume = std::clamp(volume, 0.0f, 100.0f);;

            // check to see if it's either a music clip or sound effect clip
            if (soundData.IsSoundValid())
            {
                soundData.sound.setVolume(clampedVolume);
            }
            else if (soundData.IsMusicValid())
            {
                soundData.music->SetVolume(clampedVolume);
            }
		}
	}

	void AudioEngine::SetAudioPitch(uint8_t playingID, float pitch)
	{
        // check if the requested audio is playing
		if (m_CurrentPlayingAudio.contains(playingID))
		{
			auto& soundData = m_CurrentPlayingAudio.at(playingID);

            // check to see if it's either a music clip or sound effect clip
            if (soundData.IsSoundValid())
            {
                soundData.sound.setPitch(pitch);
            }
            else if (soundData.IsMusicValid())
            {
                soundData.music->SetPitch(pitch);
            }
		}
	}

	void AudioEngine::SetAudioPosition(uint8_t playingID, float x, float y, float depth, float minDistance, float maxDistance)
	{
        // check if the requested audio is playing
		if (m_CurrentPlayingAudio.contains(playingID))
		{
			auto& soundData = m_CurrentPlayingAudio.at(playingID);

            // check to see if it's either a music clip or sound effect clip
            if (soundData.IsSoundValid())
            {
                auto& sound = soundData.sound;

                // sounds should always be at position 0 if its relative, so it acts as 2D
                if (!sound.isRelativeToListener())
                {
                    sound.setPosition(x, y, depth);
                    sound.setMinDistance(minDistance);
                    sound.setMinDistance(maxDistance);
                }
            }
            else if (soundData.IsMusicValid())
            {
                auto& music = soundData.music;

                // sounds should always be at position 0 if its relative, so it acts as 2D
                if (!music->IsRelativeToListener())
                {
                    music->SetPosition(x, y, depth);
                    music->SetMinDistance(minDistance);
                    music->SetMinDistance(maxDistance);
                }
            }
		}
	}

	void AudioEngine::SetSpatializationMode(uint8_t playingID, bool isSpatialization)
	{
        // check if the requested audio is playing
        if (m_CurrentPlayingAudio.contains(playingID))
        {
            auto& soundData = m_CurrentPlayingAudio.at(playingID);

            // check to see if it's either a music clip or sound effect clip
            if (soundData.IsSoundValid())
            {
                auto& sound = soundData.sound;

                if (isSpatialization)
                {
                    sound.setRelativeToListener(false);
                }
                else
                {
                    sound.setRelativeToListener(true);
                    sound.setPosition(0, 0, 0);
                }
            }
            else if (soundData.IsMusicValid())
            {
                auto& music = soundData.music;

                if (isSpatialization)
                {
                    music->SetRelativeToListener(false);
                }
                else
                {
                    music->SetRelativeToListener(true);
                    music->SetPosition(0, 0, 0);
                }
            }
        }
	}

    void AudioEngine::SetAudioOffsetTime(uint8_t playingID, float time)
    {
        // check if the requested audio is playing
        if (m_CurrentPlayingAudio.contains(playingID))
        {
            auto& soundData = m_CurrentPlayingAudio.at(playingID);
            bool loop = false;

            // don't need to update the position if they are "equal"
            if (std::abs(time - soundData.previousTimeOffset) < std::numeric_limits<float>::epsilon()) return;

            // check to see if it's either a music clip or sound effect clip
            if (soundData.IsSoundValid())
            {
                soundData.sound.setPlayingOffset(sf::seconds(time));

                // this shouldn't really happen, but ill place this here just in-case...
                assert(soundData.event != nullptr);

                // remove current sound event
                m_AudioEventQueue.erase(*soundData.event);

                // recreate sound event with new event time
                float playingTimeLeft = GetDuration(soundData.sound) - GetPlayingOffset(soundData.sound);
                float stopTime = soundData.sound.getLoop() ? std::numeric_limits<float>::max() : m_CurrentTime.asSeconds() + playingTimeLeft;

                m_AudioEventQueue.emplace(playingID, stopTime);
            }
            else if (soundData.IsMusicValid())
            {
                soundData.music->SetPlayingOffset(time);

                // this shouldn't really happen, but ill place this here just in-case...
                assert(soundData.event != nullptr);

                // remove current sound event
                m_AudioEventQueue.erase(*soundData.event);

                // recreate sound event with new event time
                float playingTimeLeft = GetDuration(soundData.sound) - GetPlayingOffset(soundData.sound);
                float stopTime = soundData.music->GetLoop() ? std::numeric_limits<float>::max() : m_CurrentTime.asSeconds() + playingTimeLeft;

                m_AudioEventQueue.emplace(playingID, stopTime);
            }
        }
    }

    float AudioEngine::GetAudioOffsetTime(uint8_t playingID)
    {
        // check if the requested audio is playing
        if (m_CurrentPlayingAudio.contains(playingID))
        {
            auto& soundData = m_CurrentPlayingAudio.at(playingID);
            float offset = GetPlayingOffset(soundData.sound);

            soundData.previousTimeOffset = offset;

            return offset;
        }

        return 0;
    }

	void AudioEngine::SetListenerPosition(float x, float y, float depth)
	{
        // causes backend issues if this isn't here, mainly because audio doesn't exist to offset other emitters
		if (m_CurrentPlayingAudio.empty()) return;

		sf::Listener::setPosition(x, y, depth);
	}

	void AudioEngine::SetGlobalVolume(float volume)
	{
        // causes backend issues if this isn't here, mainly because audio doesn't exist to offset other emitters
		if (m_CurrentPlayingAudio.empty()) return;

		sf::Listener::setGlobalVolume(std::clamp(volume, 0.0f, 100.0f));
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
			auto audioSourceID = m_AudioEventQueue.begin()->playingID;

			if (m_Callback != nullptr)
			{
				if (m_CurrentPlayingAudio.contains(audioSourceID))
					m_Callback->OnAudioFinish(audioSourceID, m_CurrentPlayingAudio.at(audioSourceID).userData);
			}

			m_CurrentPlayingAudio.erase(audioSourceID);
			m_AudioEventQueue.erase(m_AudioEventQueue.begin());

			ReturnID(audioSourceID);
		}
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

	float AudioEngine::GetPlayingOffset(const sf::Sound& sound)
	{
		return sound.getPlayingOffset().asSeconds();
	}

    float AudioEngine::GetPlayingOffset(const std::shared_ptr<Music>& music)
    {
        return music->GetPlayingOffset().asSeconds();
    }

	float AudioEngine::GetDuration(const sf::Sound& sound)
	{
		return sound.getBuffer()->getDuration().asSeconds();
	}

    float AudioEngine::GetDuration(const std::shared_ptr<Music>& music)
    {
        return music->GetDuration().asSeconds();
    }

	uint8_t AudioEngine::PlayAudioClip(AudioClip& clip, float volume, float pitch, bool loop, float x, float y, float depth, float minDistance, float maxDistance, const std::any& userData)
	{
        // if it is a valid audio clip
		if (clip.IsValid())
		{
			const auto& soundBuffer = static_cast<SoundBuffer&>(*clip.m_Handle); // this should be safe as we know its audio type.

			sf::Sound sound;
			sound.setBuffer(soundBuffer.GetBuffer());
			sound.setVolume(std::clamp(volume, 0.0f, 100.0f));
			sound.setPitch(pitch);
			sound.setLoop(loop);
            sound.setPosition(x, y, depth);
            sound.setMinDistance(minDistance);
            sound.setAttenuation(maxDistance);

			const float stopTime = loop ? std::numeric_limits<float>::max() : m_CurrentTime.asSeconds() + clip.GetDuration();
			const uint8_t audioSourceID = GetNextID();
			const auto& [event, successful] = m_AudioEventQueue.emplace(audioSourceID, stopTime);

            // construct audio source and play it
            m_CurrentPlayingAudio.try_emplace(audioSourceID, std::move(sound), &(*event), userData);
			m_CurrentPlayingAudio.at(audioSourceID).sound.play();

			return audioSourceID;
		}

		return c_InvalidAudioSource;
	}

	uint8_t AudioEngine::PlayStreamedAudioClip(AudioClip& clip, float volume, float pitch, bool loop, float x, float y, float depth, float minDistance, float maxDistance, const std::any& userData)
	{
        // if it is a valid audio clip
		if (clip.IsValid())
		{
			// load music source
			auto& soundReference = static_cast<SoundReference&>(*clip.m_Handle); // this should be safe as we know its audio type.
			auto music = std::make_shared<Music>();

            // since this is a wrapper to make it act like sf::Sound we have to load it
			if (!music->Load(soundReference))
			{
				return c_InvalidAudioSource;
			}

			// set source properties
			music->SetVolume(std::clamp(volume, 0.0f, 100.0f));
			music->SetPitch(pitch);
			music->SetLoop(loop);
			music->SetPosition(x, y, depth);
			music->SetMinDistance(minDistance);
			music->SetMaxDistance(maxDistance);

			const float stopTime = loop ? std::numeric_limits<float>::infinity() : m_CurrentTime.asSeconds() + clip.GetDuration();
			const uint8_t audioSourceID = GetNextID();
			const auto& [event, successful] = m_AudioEventQueue.emplace(audioSourceID, stopTime);

            // construct audio source and play it
			m_CurrentPlayingAudio.try_emplace(audioSourceID, std::move(music), &(*event), userData);
			m_CurrentPlayingAudio.at(audioSourceID).music->Play();

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

} // Mix