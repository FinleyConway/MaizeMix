#include <catch2/catch_test_macros.hpp>
#include <MaizeMix/MaizeMix.h>

#include "catch2/catch_approx.hpp"
#include "MaizeMix/Audio/Data/SoundBuffer.h"

Mix::AudioSpecification g_Spec(false, 50, 1, 0, 0, 0, 5, 10);

TEST_CASE("Audio playing")
{
	Mix::SoundHandler soundHandler;
	std::set<Mix::AudioEventData> events;
	Mix::SoundBuffer soundBuffer;
	soundBuffer.OpenFromFile("Clips/Pew.wav");

	uint64_t entity = 12345;
	uint8_t audioSourceID = 1;

	bool result = soundHandler.PlayClip(soundBuffer, g_Spec, entity, events, audioSourceID, 0.0f);

	REQUIRE(result == true);
	REQUIRE(events.size() == 1);
	REQUIRE(soundHandler.HasEmitter(audioSourceID) == true);
}

TEST_CASE("Audio pausing")
{
	Mix::SoundHandler soundHandler;
	std::set<Mix::AudioEventData> events;
	Mix::SoundBuffer soundBuffer;
	soundBuffer.OpenFromFile("Clips/Pew.wav");

	uint64_t entity = 12345;
	uint8_t audioSourceID = 1;

	soundHandler.PlayClip(soundBuffer, g_Spec, entity, events, audioSourceID, 0.0f);

	bool pause = soundHandler.PauseClip(audioSourceID, events);

	REQUIRE(pause == true);
	REQUIRE(events.size() == 0);
	REQUIRE(soundHandler.HasEmitter(audioSourceID) == true);
}

TEST_CASE("Audio un-pausing")
{
	Mix::SoundHandler soundHandler;
	std::set<Mix::AudioEventData> events;
	Mix::SoundBuffer soundBuffer;
	soundBuffer.OpenFromFile("Clips/Pew.wav");

	uint64_t entity = 12345;
	uint8_t audioSourceID = 1;

	soundHandler.PlayClip(soundBuffer, g_Spec, entity, events, audioSourceID, 0.0f);
	soundHandler.PauseClip(audioSourceID, events);

	bool unpause = soundHandler.UnPauseClip(audioSourceID, events, 0.0f);

	REQUIRE(unpause == true);
	REQUIRE(events.size() == 1);
	REQUIRE(soundHandler.HasEmitter(audioSourceID) == true);
}

TEST_CASE("Audio stopping")
{
	Mix::SoundHandler soundHandler;
	std::set<Mix::AudioEventData> events;
	Mix::SoundBuffer soundBuffer;
	soundBuffer.OpenFromFile("Clips/Pew.wav");

	uint64_t entity = 12345;
	uint8_t audioSourceID = 1;

	soundHandler.PlayClip(soundBuffer, g_Spec, entity, events, audioSourceID, 0.0f);

	soundHandler.StopClip(audioSourceID, events, [&](uint8_t id, uint64_t e) {
		REQUIRE(entity == e);
		REQUIRE(audioSourceID == id);
	});

	REQUIRE(events.size() == 0);
	REQUIRE(soundHandler.HasEmitter(audioSourceID) == false);
}

TEST_CASE("Audio play, pause, unpause, and stop")
{
	Mix::SoundHandler soundHandler;
	std::set<Mix::AudioEventData> events;
	Mix::SoundBuffer soundBuffer;
	soundBuffer.OpenFromFile("Clips/Pew.wav");

	uint64_t entity = 12345;
	uint8_t audioSourceID = 1;

	// play the audio clip
	bool playResult = soundHandler.PlayClip(soundBuffer, g_Spec, entity, events, audioSourceID, 0.0f);
	REQUIRE(playResult == true);
	REQUIRE(events.size() == 1);
	REQUIRE(soundHandler.HasEmitter(audioSourceID));

	// pause the audio clip
	bool pauseResult = soundHandler.PauseClip(audioSourceID, events);
	REQUIRE(pauseResult == true);
	REQUIRE(events.size() == 0);
	REQUIRE(soundHandler.HasEmitter(audioSourceID));

	// un-pause the audio clip
	bool unpauseResult = soundHandler.UnPauseClip(audioSourceID, events, 0.0f);
	REQUIRE(unpauseResult == true);
	REQUIRE(events.size() == 1);
	REQUIRE(soundHandler.HasEmitter(audioSourceID));

	// stop the audio clip
	soundHandler.StopClip(audioSourceID, events, [&](uint8_t id, uint64_t e)
	{
		REQUIRE(entity == e);
		REQUIRE(audioSourceID == id);
	});
	REQUIRE(events.size() == 0);
	REQUIRE(!soundHandler.HasEmitter(audioSourceID));
}

TEST_CASE("Repeated PlayClip calls")
{
	Mix::SoundHandler soundHandler;
	std::set<Mix::AudioEventData> events;
	Mix::SoundBuffer soundBuffer;
	soundBuffer.OpenFromFile("Clips/Pew.wav");

	uint64_t entity = 12345;
	uint8_t audioSourceID = 1;

	bool playResult = soundHandler.PlayClip(soundBuffer, g_Spec, entity, events, audioSourceID, 0.0f);
	REQUIRE(playResult == true);
	REQUIRE(events.size() == 1);
	REQUIRE(soundHandler.HasEmitter(audioSourceID));

	// should not alter state
	playResult = soundHandler.PlayClip(soundBuffer, g_Spec, entity, events, audioSourceID, 0.0f);
	REQUIRE(playResult == false);
	REQUIRE(events.size() == 1);
	REQUIRE(soundHandler.HasEmitter(audioSourceID));
}

TEST_CASE("Repeated PauseClip calls")
{
	Mix::SoundHandler soundHandler;
	std::set<Mix::AudioEventData> events;
	Mix::SoundBuffer soundBuffer;
	soundBuffer.OpenFromFile("Clips/Pew.wav");

	uint64_t entity = 12345;
	uint8_t audioSourceID = 1;
	soundHandler.PlayClip(soundBuffer, g_Spec, entity, events, audioSourceID, 0.0f);

	bool pauseResult = soundHandler.PauseClip(audioSourceID, events);
	REQUIRE(pauseResult == true);
	REQUIRE(events.size() == 0);
	REQUIRE(soundHandler.HasEmitter(audioSourceID));

	// should not alter state
	pauseResult = soundHandler.PauseClip(audioSourceID, events);
	REQUIRE(pauseResult == false);
	REQUIRE(events.size() == 0);
	REQUIRE(soundHandler.HasEmitter(audioSourceID));
}

TEST_CASE("Repeated UnPauseClip calls")
{
	Mix::SoundHandler soundHandler;
	std::set<Mix::AudioEventData> events;
	Mix::SoundBuffer soundBuffer;
	soundBuffer.OpenFromFile("Clips/Pew.wav");

	uint64_t entity = 12345;
	uint8_t audioSourceID = 1;
	soundHandler.PlayClip(soundBuffer, g_Spec, entity, events, audioSourceID, 0.0f);

	// pause before un-pausing
	soundHandler.PauseClip(audioSourceID, events);

	// first un-pause
	bool unpauseResult = soundHandler.UnPauseClip(audioSourceID, events, 0.0f);
	REQUIRE(unpauseResult == true);
	REQUIRE(events.size() == 1);
	REQUIRE(soundHandler.HasEmitter(audioSourceID));

	// should not alter state
	unpauseResult = soundHandler.UnPauseClip(audioSourceID, events, 0.0f);
	REQUIRE(unpauseResult == false);
	REQUIRE(events.size() == 1);
	REQUIRE(soundHandler.HasEmitter(audioSourceID));
}

TEST_CASE("Audio pause invalid handling")
{
	Mix::SoundHandler soundHandler;
	std::set<Mix::AudioEventData> events;
	uint64_t entity = 12345;
	uint8_t audioSourceID = 1;

	{
		Mix::SoundBuffer soundBuffer;
		soundBuffer.OpenFromFile("Clips/Pew.wav");
		soundHandler.PlayClip(soundBuffer, g_Spec, entity, events, audioSourceID, 0.0f);
	}

	bool pauseResult = soundHandler.PauseClip(audioSourceID, events);
	REQUIRE(pauseResult == false);
	REQUIRE(events.size() == 0);
	REQUIRE(!soundHandler.HasEmitter(audioSourceID));
}

TEST_CASE("Audio unpause invalid handling")
{
	Mix::SoundHandler soundHandler;
	std::set<Mix::AudioEventData> events;
	uint64_t entity = 12345;
	uint8_t audioSourceID = 1;

	{
		Mix::SoundBuffer soundBuffer;
		soundBuffer.OpenFromFile("Clips/Pew.wav");
		soundHandler.PlayClip(soundBuffer, g_Spec, entity, events, audioSourceID, 0.0f);
	}

	bool unpauseResult = soundHandler.UnPauseClip(audioSourceID, events, 0.0f);
	REQUIRE(unpauseResult == false);
	REQUIRE(events.size() == 0);
	REQUIRE(!soundHandler.HasEmitter(audioSourceID));
}

TEST_CASE("Audio stop invalid handling")
{
	Mix::SoundHandler soundHandler;
	std::set<Mix::AudioEventData> events;
	uint64_t entity = 12345;
	uint8_t audioSourceID = 1;

	{
		Mix::SoundBuffer soundBuffer;
		soundBuffer.OpenFromFile("Clips/Pew.wav");
		soundHandler.PlayClip(soundBuffer, g_Spec, entity, events, audioSourceID, 0.0f);
	}

	soundHandler.StopClip(audioSourceID, events, [&](uint8_t id, uint64_t e)
	{
		REQUIRE(entity == e);
		REQUIRE(audioSourceID == id);
	});

	REQUIRE(events.size() == 0);
	REQUIRE(!soundHandler.HasEmitter(audioSourceID));
}
