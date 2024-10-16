#pragma once

#include <unordered_map>
#include <string>
#include <memory>

namespace Mix {

    class Clip;
    class AudioClip;

    class AudioManager
    {
    public:
        AudioClip CreateClip(const std::string& filePath, bool stream);
        void DestroyClip(AudioClip& clip);

    private:
        std::unordered_map<size_t, std::shared_ptr<Clip>> m_AudioClips;
        static constexpr uint8_t c_InvalidClip = 0;
    };

} // Mix