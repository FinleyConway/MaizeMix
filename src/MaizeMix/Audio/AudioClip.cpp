#include "MaizeMix/Audio/AudioClip.h"

namespace Mix {

    uint32_t AudioClip::GetChannel() const
    {
        if (const auto handle = m_Handle.lock())
        {
            return handle->GetChannelCount();
        }

        return 0;
    }

    float AudioClip::GetDuration() const
    {
        if (const auto handle = m_Handle.lock())
        {
            return handle->GetDuration().asSeconds();
        }

        return 0.0f;
    }

    uint32_t AudioClip::GetFrequency() const
    {
        if (const auto handle = m_Handle.lock())
        {
            return handle->GetSampleRate();
        }

        return 0;
    }

    uint64_t AudioClip::GetSampleCount() const
    {
        if (const auto handle = m_Handle.lock())
        {
            return handle->GetSampleCount();
        }

        return 0;
    }

    bool AudioClip::IsLoadInBackground() const
    {
        return m_IsStreaming;
    }

    AudioClip::LoadState AudioClip::GetLoadState() const
    {
        return m_LoadState;
    }

    bool AudioClip::IsValid() const
    {
        return !m_Handle.expired();
    }

} // Mix