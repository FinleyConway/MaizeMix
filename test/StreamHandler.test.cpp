#include <catch2/catch_test_macros.hpp>
#include <MaizeMix/MaizeMix.h>

#include "MaizeMix/Audio/Data/SoundReference.h"
#include "MaizeMix/Engine/EngineHandler/StreamHandler.h"

TEST_CASE("Playing", "[Stream]")
{
	Mix::StreamHandler soundHandler;
	std::set<Mix::AudioEventData> events;
	Mix::SoundReference soundBuffer;
	soundBuffer.OpenFromFile("Clips/Pew.wav");

	uint64_t entity = 12345;
	uint8_t audioSourceID = 1;

	bool result = soundHandler.PlayClip(soundBuffer, Mix::AudioSpecification(false, 50, 1, 0, 0, 0, 5, 10), entity, events, audioSourceID, 0.0f);

	REQUIRE(result == true);
	REQUIRE(events.size() == 1);
	REQUIRE(soundHandler.HasEmitter(audioSourceID) == true);
}

TEST_CASE("Pausing", "[Stream]")
{
	Mix::StreamHandler soundHandler;
	std::set<Mix::AudioEventData> events;
	Mix::SoundReference soundBuffer;
	soundBuffer.OpenFromFile("Clips/Pew.wav");

	uint64_t entity = 12345;
	uint8_t audioSourceID = 1;

	soundHandler.PlayClip(soundBuffer, Mix::AudioSpecification(false, 50, 1, 0, 0, 0, 5, 10), entity, events, audioSourceID, 0.0f);

	bool pause = soundHandler.PauseClip(audioSourceID, events);

	REQUIRE(pause == true);
	REQUIRE(events.size() == 0);
	REQUIRE(soundHandler.HasEmitter(audioSourceID) == true);
}

TEST_CASE("Un-pausing", "[Stream]")
{
	Mix::StreamHandler soundHandler;
	std::set<Mix::AudioEventData> events;
	Mix::SoundReference soundBuffer;
	soundBuffer.OpenFromFile("Clips/Pew.wav");

	uint64_t entity = 12345;
	uint8_t audioSourceID = 1;

	soundHandler.PlayClip(soundBuffer, Mix::AudioSpecification(false, 50, 1, 0, 0, 0, 5, 10), entity, events, audioSourceID, 0.0f);
	soundHandler.PauseClip(audioSourceID, events);

	bool unpause = soundHandler.UnPauseClip(audioSourceID, events, 0.0f);

	REQUIRE(unpause == true);
	REQUIRE(events.size() == 1);
	REQUIRE(soundHandler.HasEmitter(audioSourceID) == true);
}

TEST_CASE("Stopping", "[Stream]")
{
	Mix::StreamHandler soundHandler;
	std::set<Mix::AudioEventData> events;
	Mix::SoundReference soundBuffer;
	soundBuffer.OpenFromFile("Clips/Pew.wav");

	uint64_t entity = 12345;
	uint8_t audioSourceID = 1;

	soundHandler.PlayClip(soundBuffer, Mix::AudioSpecification(false, 50, 1, 0, 0, 0, 5, 10), entity, events, audioSourceID, 0.0f);

	soundHandler.StopClip(audioSourceID, events, [&](uint8_t id, uint64_t e) {
		REQUIRE(entity == e);
		REQUIRE(audioSourceID == id);
	});

	REQUIRE(events.size() == 0);
	REQUIRE(soundHandler.HasEmitter(audioSourceID) == false);
}

TEST_CASE("Play, pause, unpause, and stop", "[Stream]")
{
	Mix::StreamHandler soundHandler;
	std::set<Mix::AudioEventData> events;
	Mix::SoundReference soundBuffer;
	soundBuffer.OpenFromFile("Clips/Pew.wav");

	uint64_t entity = 12345;
	uint8_t audioSourceID = 1;

	// play the audio clip
	bool playResult = soundHandler.PlayClip(soundBuffer, Mix::AudioSpecification(false, 50, 1, 0, 0, 0, 5, 10), entity, events, audioSourceID, 0.0f);

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

TEST_CASE("Repeated PlayClip calls", "[Stream]")
{
	Mix::StreamHandler soundHandler;
	std::set<Mix::AudioEventData> events;
	Mix::SoundReference soundBuffer;
	soundBuffer.OpenFromFile("Clips/Pew.wav");

	uint64_t entity = 12345;
	uint8_t audioSourceID = 1;

	bool playResult = soundHandler.PlayClip(soundBuffer, Mix::AudioSpecification(false, 50, 1, 0, 0, 0, 5, 10), entity, events, audioSourceID, 0.0f);

	REQUIRE(playResult == true);
	REQUIRE(events.size() == 1);
	REQUIRE(soundHandler.HasEmitter(audioSourceID));

	// should not alter state
	playResult = soundHandler.PlayClip(soundBuffer, Mix::AudioSpecification(false, 50, 1, 0, 0, 0, 5, 10), entity, events, audioSourceID, 0.0f);

	REQUIRE(playResult == false);
	REQUIRE(events.size() == 1);
	REQUIRE(soundHandler.HasEmitter(audioSourceID));
}

TEST_CASE("Repeated PauseClip calls", "[Stream]")
{
	Mix::StreamHandler soundHandler;
	std::set<Mix::AudioEventData> events;
	Mix::SoundReference soundBuffer;
	soundBuffer.OpenFromFile("Clips/Pew.wav");

	uint64_t entity = 12345;
	uint8_t audioSourceID = 1;
	soundHandler.PlayClip(soundBuffer, Mix::AudioSpecification(false, 50, 1, 0, 0, 0, 5, 10), entity, events, audioSourceID, 0.0f);

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

TEST_CASE("Repeated UnPauseClip calls", "[Stream]")
{
	Mix::StreamHandler soundHandler;
	std::set<Mix::AudioEventData> events;
	Mix::SoundReference soundBuffer;
	soundBuffer.OpenFromFile("Clips/Pew.wav");

	uint64_t entity = 12345;
	uint8_t audioSourceID = 1;
	soundHandler.PlayClip(soundBuffer, Mix::AudioSpecification(false, 50, 1, 0, 0, 0, 5, 10), entity, events, audioSourceID, 0.0f);

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

TEST_CASE("Pause invalid handling", "[Stream]")
{
	Mix::StreamHandler soundHandler;
	std::set<Mix::AudioEventData> events;
	uint64_t entity = 12345;
	uint8_t audioSourceID = 1;

	{
		Mix::SoundReference soundBuffer;
		soundBuffer.OpenFromFile("Clips/Pew.wav");
		soundHandler.PlayClip(soundBuffer, Mix::AudioSpecification(false, 50, 1, 0, 0, 0, 5, 10), entity, events, audioSourceID, 0.0f);
	}

	bool pauseResult = soundHandler.PauseClip(audioSourceID, events);

	REQUIRE(pauseResult == false);
	REQUIRE(events.size() == 0);
	REQUIRE(!soundHandler.HasEmitter(audioSourceID));
}

TEST_CASE("Unpause invalid handling", "[Stream]")
{
	Mix::StreamHandler soundHandler;
	std::set<Mix::AudioEventData> events;
	uint64_t entity = 12345;
	uint8_t audioSourceID = 1;

	{
		Mix::SoundReference soundBuffer;
		soundBuffer.OpenFromFile("Clips/Pew.wav");
		soundHandler.PlayClip(soundBuffer, Mix::AudioSpecification(false, 50, 1, 0, 0, 0, 5, 10), entity, events, audioSourceID, 0.0f);
	}

	bool unpauseResult = soundHandler.UnPauseClip(audioSourceID, events, 0.0f);

	REQUIRE(unpauseResult == false);
	REQUIRE(events.size() == 0);
	REQUIRE(!soundHandler.HasEmitter(audioSourceID));
}

TEST_CASE("Stop invalid handling", "[Stream]")
{
	Mix::StreamHandler soundHandler;
	std::set<Mix::AudioEventData> events;
	uint64_t entity = 12345;
	uint8_t audioSourceID = 1;

	{
		Mix::SoundReference soundBuffer;
		soundBuffer.OpenFromFile("Clips/Pew.wav");
		soundHandler.PlayClip(soundBuffer, Mix::AudioSpecification(false, 50, 1, 0, 0, 0, 5, 10), entity, events, audioSourceID, 0.0f);
	}

	soundHandler.StopClip(audioSourceID, events, [&](uint8_t id, uint64_t e)
	{
		REQUIRE(entity == e);
		REQUIRE(audioSourceID == id);
	});

	REQUIRE(events.size() == 0);
	REQUIRE(!soundHandler.HasEmitter(audioSourceID));
}
