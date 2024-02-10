#pragma once

#include "entt/entt.hpp"
#include "MaizeMix.h"

#include "test_Components.h"

namespace Maize {

	class test_AudioSystem
	{
	 public:
		void Update(float deltaTime, entt::registry& reg)
		{
			m_AudioEngine.Update(deltaTime);

			PlayAudio(reg);
			StopAudio(reg);
		}

	 private:
		void PlayAudio(entt::registry& reg)
		{
			auto view = reg.view<PositionComponent, AudioSourceComponent, PlayAudioSourceTag>();
			for (auto [entity, position, audio] : view.each())
			{
				if (audio.spatialize)
				{
					auto id = m_AudioEngine.PlaySoundAtPosition(audio.clip, audio.volume, audio.pitch, audio.loop,
						position.x, position.y, 0.0f, audio.minDistance, audio.maxDistance);

					reg.emplace<PlayingAudioSourceTag>(entity).audioSourceID = id;
				}
				else
				{
					auto id = m_AudioEngine.PlaySound(audio.clip, audio.volume, audio.pitch, audio.loop);

					reg.emplace<PlayingAudioSourceTag>(entity).audioSourceID = id;
				}
			}
		}
		
		void StopAudio(entt::registry& reg)
		{
			auto view = reg.view<PositionComponent, AudioSourceComponent, PlayingAudioSourceTag, StopAudioSourceTag>();
			for (auto [entity, position, audio, playingID] : view.each())
			{
				// dont like, lets just mae a StopSound function
				m_AudioEngine.SetAudioState(playingID.audioSourceID, Mix::AudioState::Stop);

				reg.remove<PlayingAudioSourceTag>(entity);
				reg.remove<StopAudioSourceTag>(entity);
			}
		}

		Mix::AudioEngine m_AudioEngine;
	};

}