#pragma once

#include <iostream>

#include "entt/entt.hpp"
#include "MaizeMix.h"

#include "test_Components.h"

namespace Maize {

	class test_AudioSystem
	{
	 public:
		void Update(float deltaTime, entt::registry& reg, Mix::AudioEngine& engine)
		{
			engine.Update(deltaTime);

			auto view = reg.view<PositionComponent, AudioSourceComponent>();
			for (auto [entity, position, audio] : view.each())
			{
				// audio source tagged to be played
				if (reg.all_of<PlayAudioSourceTag>(entity))
				{
					PlayAudio(reg, entity, engine);
				}

				// if the audio is playing
				if (reg.all_of<PlayingAudioSourceTag>(entity))
				{
					auto& playing = reg.get<PlayingAudioSourceTag>(entity);

					SetAudioAttributes(reg, entity, engine);

					if (reg.all_of<StopAudioSourceTag>(entity))
					{
						engine.StopAudio(playing.playingID);

						reg.remove<StopAudioSourceTag>(entity);

						continue;
					}

					if (reg.all_of<PauseAudioSourceTag>(entity))
					{
						engine.PauseAudio(playing.playingID);

						reg.remove<PauseAudioSourceTag>(entity);
					}

					if (reg.all_of<UnPauseAudioSourceTag>(entity))
					{
						engine.UnpauseAudio(playing.playingID);

						reg.remove<UnPauseAudioSourceTag>(entity);
					}
				}
				else
				{
					// remove any added audio based components if the audio source is not playing
					reg.remove<PauseAudioSourceTag>(entity);
					reg.remove<UnPauseAudioSourceTag>(entity);
					reg.remove<StopAudioSourceTag>(entity);
				}
			}

		}

	 private:
        void SetAudioAttributes(entt::registry& reg, entt::entity entity, Mix::AudioEngine& engine)
        {
			const auto& position = reg.get<PositionComponent>(entity);
			auto& audio = reg.get<AudioSourceComponent>(entity);
			auto& playing = reg.get<PlayingAudioSourceTag>(entity);

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
        }

		void PlayAudio(entt::registry& reg, entt::entity entity, Mix::AudioEngine& engine)
		{
			const auto& position = reg.get<PositionComponent>(entity);
			auto& audio = reg.get<AudioSourceComponent>(entity);

			auto userData = entity;

			// stop current playing audio its playing
			if (reg.all_of<PlayingAudioSourceTag>(entity))
			{
				auto& playing = reg.get<PlayingAudioSourceTag>(entity);

				engine.StopAudio(playing.playingID);
			}

			reg.remove<PauseAudioSourceTag>(entity);

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
	};

}