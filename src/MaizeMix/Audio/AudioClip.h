#pragma once

#include <memory>

#include "MaizeMix/Audio/Data/Clip.h"

namespace Mix {

    class AudioClip
    {
    public:
        AudioClip() = default;

        enum class LoadState { Unloaded = 0, Loaded, Failed };

        uint32_t GetChannel() const;
        float GetDuration() const;
        uint32_t GetFrequency() const;
        uint64_t GetSampleCount() const;
        bool IsLoadInBackground() const;
        LoadState GetLoadState() const;
        bool IsValid() const;

    private:
        friend class AudioEngine;
        friend class AudioManager;

        AudioClip(size_t clipID, const std::shared_ptr<Clip>& clip, bool stream, LoadState loadState) :
            m_ClipID(clipID), m_Handle(clip), m_IsStreaming(stream), m_LoadState(loadState)
        {
        }

        size_t m_ClipID = 0;
        std::weak_ptr<Clip> m_Handle;

        bool m_IsStreaming = false;
        LoadState m_LoadState = LoadState::Unloaded;
    };

} // Mix