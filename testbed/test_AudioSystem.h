#pragma once

#include "entt/entt.hpp"
#include "MaizeMix.h"

#include "test_Components.h"

namespace Maize {

    /*
     * TODO:
     * Switch between 2d and 3d sounds (make sounds relative to listener)
     */

	class test_AudioSystem
	{
	 public:
		void Update(float deltaTime, entt::registry& reg, Mix::AudioEngine& engine)
		{
			auto view = reg.view<PlayingAudioSourceTag>();
			for (auto [entity, playing] : view.each())
			{
				printf("?\n");
			}

			engine.Update(deltaTime);

            SetAudioAttributes(reg, engine);

			PlayAudio(reg, engine);
            PauseAudio(reg, engine);
            UnpauseAudio(reg, engine);
			StopAudio(reg, engine);
		}

	 private:
        void SetAudioAttributes(entt::registry& reg, Mix::AudioEngine& engine)
        {
            auto view = reg.view<PositionComponent, AudioSourceComponent, PlayingAudioSourceTag>();
            for (auto [entity, position, audio, playingID] : view.each())
            {
                engine.SetAudioLoopState(playingID.audioSourceID, audio.loop);
                engine.SetAudioMuteState(playingID.audioSourceID, audio.mute);
                engine.SetAudioPitch(playingID.audioSourceID, audio.pitch);
                engine.SetAudioVolume(playingID.audioSourceID, audio.volume);

                if (audio.spatialize)
                {
                    engine.SetAudioPosition(playingID.audioSourceID, position.x, position.y, 0, audio.minDistance, audio.maxDistance);
                }
            }
        }

		void PlayAudio(entt::registry& reg, Mix::AudioEngine& engine)
		{
			auto view = reg.view<PositionComponent, AudioSourceComponent, PlayAudioSourceTag>();
			for (auto [entity, position, audio] : view.each())
			{
				if (audio.spatialize)
				{
					auto userData = entity;
					auto id = engine.PlayAudio(audio.clip, audio.volume, audio.pitch, audio.loop, userData,
                                                                position.x, position.y, 0.0f, audio.minDistance,
                                                                audio.maxDistance);

					reg.emplace_or_replace<PlayingAudioSourceTag>(entity, id);
					reg.remove<PlayAudioSourceTag>(entity);
				}
				else
				{
					auto userData = entity;
					auto id = engine.PlayAudio(audio.clip, audio.volume, audio.pitch, audio.loop, userData);

					reg.emplace_or_replace<PlayingAudioSourceTag>(entity, id);
					reg.remove<PlayAudioSourceTag>(entity);
				}
			}
		}

        void PauseAudio(entt::registry& reg, Mix::AudioEngine& engine)
        {
            auto view = reg.view<AudioSourceComponent, PlayingAudioSourceTag, PauseAudioSourceTag>();
            for (auto [entity, audio, playingID] : view.each())
            {
                engine.PauseAudio(playingID.audioSourceID);
            }
        }

        void UnpauseAudio(entt::registry& reg, Mix::AudioEngine& engine)
        {
            auto view = reg.view<AudioSourceComponent, PlayingAudioSourceTag, PauseAudioSourceTag, PlayAudioSourceTag>();
            for (auto [entity, audio, playingID] : view.each())
            {
                engine.UnpauseAudio(playingID.audioSourceID);

                reg.remove<PauseAudioSourceTag>(entity);
                reg.remove<PlayAudioSourceTag>(entity);
            }
        }

		void StopAudio(entt::registry& reg, Mix::AudioEngine& engine)
		{
			auto view = reg.view<AudioSourceComponent, PlayingAudioSourceTag, StopAudioSourceTag>();
			for (auto [entity, audio, playingID] : view.each())
			{
				engine.StopAudio(playingID.audioSourceID);

				reg.remove<PlayingAudioSourceTag>(entity);
				reg.remove<StopAudioSourceTag>(entity);
			}
		}
	};

}