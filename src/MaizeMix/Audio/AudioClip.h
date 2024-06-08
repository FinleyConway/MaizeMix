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
        void GetData(std::vector<int16_t>& samples, uint64_t offset);

    private:
        friend class AudioEngine;
        friend class AudioManager;

        AudioClip(size_t clipID, const std::shared_ptr<Clip>& clip, bool stream, LoadState loadState) :
            m_ClipID(clipID), m_Handle(clip), m_IsStreaming(stream), m_LoadState(loadState)
        {
        }

        bool IsValid() const { return m_ClipID != 0 && m_Handle != nullptr; }

        size_t m_ClipID = 0;
        std::shared_ptr<Clip> m_Handle = nullptr;

        bool m_IsStreaming = false;
        LoadState m_LoadState = LoadState::Unloaded;
    };

} // Mix