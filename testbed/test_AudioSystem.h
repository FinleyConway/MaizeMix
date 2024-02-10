#pragma once

#include "entt/entt.hpp"
#include "MaizeMix.h"

#include "test_Components.h"

namespace Maize {

    /*
     * TODO:
     * Look into pause ecs functionality since its a bit iffy, needs testing
     * Switch between 2d and 3d sounds (make sounds relative to listener)
     */

	class test_AudioSystem
	{
	 public:
		void Update(float deltaTime, entt::registry& reg)
		{
			m_AudioEngine.Update(deltaTime);

            SetAudioAttributes(reg);

			PlayAudio(reg);
            PauseAudio(reg);
            UnpauseAudio(reg);
			StopAudio(reg);
		}

	 private:
        void SetAudioAttributes(entt::registry& reg)
        {
            auto view = reg.view<PositionComponent, AudioSourceComponent, PlayingAudioSourceTag>();
            for (auto [entity, position, audio, playingID] : view.each())
            {
                m_AudioEngine.SetAudioLoopState(playingID.audioSourceID, audio.loop);
                m_AudioEngine.SetAudioMuteState(playingID.audioSourceID, audio.mute);
                m_AudioEngine.SetAudioPitch(playingID.audioSourceID, audio.pitch);
                m_AudioEngine.SetAudioVolume(playingID.audioSourceID, audio.volume);

                if (audio.spatialize)
                {
                    m_AudioEngine.SetAudioPosition(playingID.audioSourceID, position.x, position.y, 0, audio.minDistance, audio.maxDistance);
                }
            }
        }

		void PlayAudio(entt::registry& reg)
		{
			auto view = reg.view<PositionComponent, AudioSourceComponent, PlayAudioSourceTag>();
			for (auto [entity, position, audio] : view.each())
			{
				if (audio.spatialize)
				{
					auto id = m_AudioEngine.PlayAudioAtPosition(audio.clip, audio.volume, audio.pitch, audio.loop,
                                                                position.x, position.y, 0.0f, audio.minDistance,
                                                                audio.maxDistance);

					reg.emplace<PlayingAudioSourceTag>(entity, id);
				}
				else
				{
					auto id = m_AudioEngine.PlayAudio(audio.clip, audio.volume, audio.pitch, audio.loop);

					reg.emplace<PlayingAudioSourceTag>(entity, id);
				}
			}
		}

        void PauseAudio(entt::registry& reg)
        {
            auto view = reg.view<AudioSourceComponent, PlayingAudioSourceTag, PauseAudioSourceTag>();
            for (auto [entity, audio, playingID] : view.each())
            {
                m_AudioEngine.PauseAudio(playingID.audioSourceID);
            }
        }

        void UnpauseAudio(entt::registry& reg)
        {
            auto view = reg.view<AudioSourceComponent, PlayingAudioSourceTag, PauseAudioSourceTag, PlayAudioSourceTag>();
            for (auto [entity, audio, playingID] : view.each())
            {
                m_AudioEngine.UnpauseAudio(playingID.audioSourceID);

                reg.remove<PauseAudioSourceTag>(entity);
                reg.remove<PlayAudioSourceTag>(entity);
            }
        }

		void StopAudio(entt::registry& reg)
		{
			auto view = reg.view<AudioSourceComponent, PlayingAudioSourceTag, StopAudioSourceTag>();
			for (auto [entity, audio, playingID] : view.each())
			{
				m_AudioEngine.StopAudio(playingID.audioSourceID);

				reg.remove<PlayingAudioSourceTag>(entity);
				reg.remove<StopAudioSourceTag>(entity);
			}
		}

    private:
		Mix::AudioEngine m_AudioEngine;
	};

}