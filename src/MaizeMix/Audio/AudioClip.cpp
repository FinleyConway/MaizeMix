#include "MaizeMix/Audio/AudioClip.h"

namespace Mix {

    uint32_t AudioClip::GetChannel() const
    {
        if (m_Handle != nullptr)
        {
            return m_Handle->GetChannelCount();
        }

        return 0;
    }

    float AudioClip::GetDuration() const
    {
        if (m_Handle != nullptr)
        {
            return m_Handle->GetDuration().asSeconds();
        }

        return 0.0f;
    }

    uint32_t AudioClip::GetFrequency() const
    {
        if (m_Handle != nullptr)
        {
            m_Handle->GetSampleRate();
        }

        return 0;
    }

    uint64_t AudioClip::GetSampleCount() const
    {
        if (m_Handle != nullptr)
        {
            m_Handle->GetSampleCount();
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

} // Mix