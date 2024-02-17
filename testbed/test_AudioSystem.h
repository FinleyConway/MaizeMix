#pragma once

#include "entt/entt.hpp"
#include "MaizeMix.h"

#include "test_Components.h"

namespace Maize {

    /*
     * TODO:
	 * find solution to prevent unnecessary adding of components
     * Switch between 2d and 3d sounds (make sounds relative to listener)
     */

	class test_AudioSystem
	{
	 public:
		void Update(float deltaTime, entt::registry& reg, Mix::AudioEngine& engine)
		{
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
            for (auto [entity, position, audio, playing] : view.each())
            {
                engine.SetAudioLoopState(playing.playingID, audio.loop);
                engine.SetAudioMuteState(playing.playingID, audio.mute);
                engine.SetAudioPitch(playing.playingID, audio.pitch);
                engine.SetAudioVolume(playing.playingID, audio.volume);

				engine.SetSpatializationMode(playing.playingID, audio.spatialize);
                if (audio.spatialize)
                {
                    engine.SetAudioPosition(playing.playingID, position.x, position.y, 0, audio.minDistance, audio.maxDistance);
                }

				audio.time = engine.GetAudioOffsetTime(playing.playingID);

				printf("%1.5f\n", audio.time);
            }
        }

		void PlayAudio(entt::registry& reg, Mix::AudioEngine& engine)
		{
			auto view = reg.view<PositionComponent, AudioSourceComponent, PlayAudioSourceTag>();
			for (auto [entity, position, audio] : view.each())
			{
				auto userData = entity;

				// stop current playing audio
				if (reg.all_of<PlayingAudioSourceTag>(entity))
				{
					auto& playing = reg.get<PlayingAudioSourceTag>(entity);

					engine.StopAudio(playing.playingID);
				}

				if (audio.spatialize)
				{
					auto id = engine.PlayAudio(audio.clip, audio.volume, audio.pitch, audio.loop, userData,
                                                                position.x, position.y, 0.0f, audio.minDistance,
                                                                audio.maxDistance);

					reg.emplace_or_replace<PlayingAudioSourceTag>(entity, id);
					reg.remove<PlayAudioSourceTag>(entity);
				}
				else
				{
					auto id = engine.PlayAudio(audio.clip, audio.volume, audio.pitch, audio.loop, userData);

					reg.emplace_or_replace<PlayingAudioSourceTag>(entity, id);
					reg.remove<PlayAudioSourceTag>(entity);
				}
			}
		}

        void PauseAudio(entt::registry& reg, Mix::AudioEngine& engine)
        {
            auto view = reg.view<AudioSourceComponent, PlayingAudioSourceTag, PauseAudioSourceTag>();
            for (auto [entity, audio, playing] : view.each())
            {
                engine.PauseAudio(playing.playingID);
            }
        }

        void UnpauseAudio(entt::registry& reg, Mix::AudioEngine& engine)
        {
            auto view = reg.view<AudioSourceComponent, PlayingAudioSourceTag, PauseAudioSourceTag, UnPauseAudioSourceTag>();
            for (auto [entity, audio, playing] : view.each())
            {
                engine.UnpauseAudio(playing.playingID);

                reg.remove<PauseAudioSourceTag>(entity);
                reg.remove<UnPauseAudioSourceTag>(entity);
            }
        }

		void StopAudio(entt::registry& reg, Mix::AudioEngine& engine)
		{
			auto view = reg.view<AudioSourceComponent, PlayingAudioSourceTag, StopAudioSourceTag>();
			for (auto [entity, audio, playing] : view.each())
			{
				engine.StopAudio(playing.playingID);

				reg.remove<StopAudioSourceTag>(entity);
			}
		}
	};

}