#pragma once

#include <SFML/Audio.hpp>

#include <functional>
#include <limits>
#include <variant>
#include <memory>
#include <set>

#include "MaizeMix/Helper/AudioClips/SoundReference.h"
#include "MaizeMix/Helper/AudioClips/SoundBuffer.h"
#include "MaizeMix/Helper/AudioSpecification.h"
#include "MaizeMix/Helper/AudioManager.h"
#include "MaizeMix/Helper/Music.h"

namespace Mix {

	class AudioClip;
	class AudioFinishCallback;

	class AudioEngine
	{
	struct Source;
	struct AudioEventData;

	public:
		AudioClip CreateClip(const std::string& filePath, bool stream);

		void RemoveClip(AudioClip& clip);

		bool PlayAudio(uint64_t entityID, const AudioClip& clip, const AudioSpecification& spec);

		bool PauseAudio(uint64_t entityID);

		bool UnpauseAudio(uint64_t entityID);

		bool StopAudio(uint64_t entityID);

		bool SetAudioLoopState(uint64_t entityID, bool loop);

		bool SetAudioMuteState(uint64_t entityID, bool mute);

		bool SetAudioVolume(uint64_t entityID, float volume);

		bool SetAudioPitch(uint64_t entityID, float pitch);

        bool SetAudioOffsetTime(uint64_t entityID, float time);

		float GetAudioOffsetTime(uint64_t entityID);

		bool SetListenerPosition(float x, float y, float depth) const;

		bool SetGlobalVolume(float volume) const;

		void SetAudioFinishCallback(std::function<void(uint64_t)>&& callback);

		bool HasHitMaxAudioSources() const;

		uint8_t EmitterCount() const;

		void Update(float deltaTime);

	private:
		using EventIterator = std::set<AudioEventData>::iterator;

		struct Source
		{
			std::variant<sf::Sound, Music> source;

			uint64_t entity = 0;
			EventIterator iterator;

			bool isMute = false;
			float previousTimeOffset = 0;

			Source(EventIterator event, uint64_t entity) : entity(entity), iterator(event) { }

			bool IsValid() const
			{
				if (const auto* sound = std::get_if<sf::Sound>(&source)) return sound->getBuffer() != nullptr;
				if (const auto* music = std::get_if<Music>(&source)) return music->getReference() != nullptr;

				return false;
			}

			float GetDuration() const
			{
				if (const auto* sound = std::get_if<sf::Sound>(&source)) return sound->getBuffer()->getDuration().asSeconds();
				if (const auto* music = std::get_if<Music>(&source)) return music->getReference()->GetDuration().asSeconds();

				return 0.0f;
			}
		};

		struct AudioEventData
		{
			const uint64_t entityID = 0;
			const float stopTime = 0;

			AudioEventData() = default;
			AudioEventData(uint64_t entityID, float stopTime)
				: entityID(entityID), stopTime(stopTime)
			{
			}

			bool operator<(const AudioEventData& other) const
			{
				if (entityID == other.entityID)
				{
					return stopTime < other.stopTime;
				}

				return entityID < other.entityID;
			}
		};

	private:
		bool RequeueAudioClip(uint64_t entityID, float duration, float playingOffset, bool isLooping, float currentTime, Source& source);

		void HandleInvalid(uint64_t entityID, EventIterator it);

		template <typename T>
		bool PlayClip(uint64_t entityID, const T& clip, const AudioSpecification& specification, std::set<AudioEventData>& event, float currentTime)
		{
			const float stopTime = specification.loop ? std::numeric_limits<float>::max() : currentTime + clip.GetDuration().asSeconds();
			const auto [it, successful] = event.emplace(entityID, stopTime);

			if (!successful) return false; // duplicate id

			m_CurrentPlayingAudio.try_emplace(entityID, it, entityID);
			auto& soundVariant = m_CurrentPlayingAudio.at(entityID).source;

			// set up audio source and specific settings
			if constexpr (std::is_same_v<T, SoundBuffer>)
			{
				soundVariant.emplace<sf::Sound>();
				auto& sound = std::get<sf::Sound>(soundVariant);

				sound.setBuffer(clip.GetBuffer());
				sound.setVolume(specification.mute ? 0.0f : std::clamp(specification.volume, 0.0f, 100.0f));
				sound.setPitch(std::max(0.0001f, specification.pitch));
				sound.setLoop(specification.loop);
				sound.play();
			}
			else if constexpr (std::is_same_v<T, SoundReference>)
			{
				soundVariant.emplace<Music>();
				auto& stream = std::get<Music>(soundVariant);

				if (!stream.setSoundReference(clip)) return false;
				stream.setVolume(specification.mute ? 0.0f : std::clamp(specification.volume, 0.0f, 100.0f));
				stream.setPitch(std::max(0.0001f, specification.pitch));
				stream.setLoop(specification.loop);
				stream.play();
			}

			return true;
		}

	private:
		sf::Time m_CurrentTime;

		AudioManager m_AudioManager;

		std::unordered_map<uint64_t, Source> m_CurrentPlayingAudio;
		std::set<AudioEventData> m_AudioEventQueue;
		std::function<void(uint64_t)> m_OnAudioFinish;

		static constexpr uint8_t c_MaxAudioEmitters = 255;
	};

} // Mix