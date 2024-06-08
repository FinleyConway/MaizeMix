#include "MaizeMix/AudioManager.h"

#include "MaizeMix/Audio/AudioClip.h"
#include "MaizeMix/Audio/SoundReference.h"
#include "MaizeMix/Audio/SoundBuffer.h"

namespace Mix {

    AudioClip AudioManager::CreateClip(const std::string &filePath, bool stream)
    {
        static size_t id = 0;

        // sfml boilerplate to create audio data
        if (stream)
        {
            auto soundReference = std::make_shared<SoundReference>();

            if (soundReference->OpenFromFile(filePath))
            {
                id++;

                auto clip = AudioClip(id, soundReference, stream, AudioClip::LoadState::Loaded);
                m_AudioClips.try_emplace(id, soundReference);

                return clip;
            }
        }
        else
        {
            auto soundBuffer = std::make_shared<SoundBuffer>();

            if (soundBuffer->OpenFromFile(filePath))
            {
                id++;

                auto clip = AudioClip(id, soundBuffer, stream, AudioClip::LoadState::Loaded);

                m_AudioClips.emplace(id, soundBuffer);

                return clip;
            }
        }

        // return a failed audio clip
        return {c_InvalidClip, nullptr, stream, AudioClip::LoadState::Failed };
    }

    void AudioManager::DestroyClip(AudioClip &clip)
    {
        // remove clip
        m_AudioClips.erase(clip.m_ClipID);

        // set clip to default
        clip = AudioClip();
    }

} // Mix