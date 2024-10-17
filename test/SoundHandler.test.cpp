#include <catch2/catch_test_macros.hpp>
#include <MaizeMix.h>

#include "MaizeMix/Sounds/SoundBuffer.h"
#include "MaizeMix/Sounds/SoundHandler.h"

TEST_CASE("Playing", "[Sound]")
{
	Mix::SoundHandler soundHandler;
	std::set<Mix::AudioEventData> events;
	Mix::SoundBuffer soundBuffer;
	soundBuffer.OpenFromFile("Clips/Pew.wav");

	uint64_t entity = 12345;

	bool result = soundHandler.PlayClip(entity, soundBuffer, Mix::AudioSpecification(false, 50, 1, 0, 0, 0, 5, 10), events, 0.0f);

	REQUIRE(result == true);
	REQUIRE(events.size() == 1);
	REQUIRE(soundHandler.HasEmitter(entity) == true);
}

TEST_CASE("Pausing", "[Sound]")
{
	Mix::SoundHandler soundHandler;
	std::set<Mix::AudioEventData> events;
	Mix::SoundBuffer soundBuffer;
	soundBuffer.OpenFromFile("Clips/Pew.wav");

	uint64_t entity = 12345;

	soundHandler.PlayClip(entity, soundBuffer, Mix::AudioSpecification(false, 50, 1, 0, 0, 0, 5, 10), events, 0.0f);

	bool pause = soundHandler.PauseClip(entity, events);

	REQUIRE(pause == true);
	REQUIRE(events.size() == 0);
	REQUIRE(soundHandler.HasEmitter(entity) == true);
}

TEST_CASE("Un-pausing", "[Sound]")
{
	Mix::SoundHandler soundHandler;
	std::set<Mix::AudioEventData> events;
	Mix::SoundBuffer soundBuffer;
	soundBuffer.OpenFromFile("Clips/Pew.wav");

	uint64_t entity = 12345;

	soundHandler.PlayClip(entity, soundBuffer, Mix::AudioSpecification(false, 50, 1, 0, 0, 0, 5, 10), events, 0.0f);
	soundHandler.PauseClip(entity, events);

	bool unpause = soundHandler.UnPauseClip(entity, events, 0.0f);

	REQUIRE(unpause == true);
	REQUIRE(events.size() == 1);
	REQUIRE(soundHandler.HasEmitter(entity) == true);
}

TEST_CASE("Stopping", "[Sound]")
{
	Mix::SoundHandler soundHandler;
	std::set<Mix::AudioEventData> events;
	Mix::SoundBuffer soundBuffer;
	soundBuffer.OpenFromFile("Clips/Pew.wav");

	uint64_t entity = 12345;

	soundHandler.PlayClip(entity, soundBuffer, Mix::AudioSpecification(false, 50, 1, 0, 0, 0, 5, 10), events, 0.0f);

	soundHandler.StopClip(entity, events, [&](uint64_t e) {
		REQUIRE(entity == e);
	});

	REQUIRE(events.size() == 0);
	REQUIRE(soundHandler.HasEmitter(entity) == false);
}

TEST_CASE("Play, pause, unpause, and stop", "[Sound]")
{
	Mix::SoundHandler soundHandler;
	std::set<Mix::AudioEventData> events;
	Mix::SoundBuffer soundBuffer;
	soundBuffer.OpenFromFile("Clips/Pew.wav");

	uint64_t entity = 12345;

	// play the audio clip
	bool playResult = soundHandler.PlayClip(entity, soundBuffer, Mix::AudioSpecification(false, 50, 1, 0, 0, 0, 5, 10), events, 0.0f);
	REQUIRE(playResult == true);
	REQUIRE(events.size() == 1);
	REQUIRE(soundHandler.HasEmitter(entity));

	// pause the audio clip
	bool pauseResult = soundHandler.PauseClip(entity, events);
	REQUIRE(pauseResult == true);
	REQUIRE(events.size() == 0);
	REQUIRE(soundHandler.HasEmitter(entity));

	// un-pause the audio clip
	bool unpauseResult = soundHandler.UnPauseClip(entity, events, 0.0f);
	REQUIRE(unpauseResult == true);
	REQUIRE(events.size() == 1);
	REQUIRE(soundHandler.HasEmitter(entity));

	// stop the audio clip
	soundHandler.StopClip(entity, events, [&](uint64_t e)
	{
		REQUIRE(entity == e);
	});
	REQUIRE(events.size() == 0);
	REQUIRE(!soundHandler.HasEmitter(entity));
}

TEST_CASE("Repeated PlayClip calls", "[Sound]")
{
	Mix::SoundHandler soundHandler;
	std::set<Mix::AudioEventData> events;
	Mix::SoundBuffer soundBuffer;
	soundBuffer.OpenFromFile("Clips/Pew.wav");

	uint64_t entity = 12345;

	bool playResult = soundHandler.PlayClip(entity, soundBuffer, Mix::AudioSpecification(false, 50, 1, 0, 0, 0, 5, 10), events, 0.0f);
	REQUIRE(playResult == true);
	REQUIRE(events.size() == 1);
	REQUIRE(soundHandler.HasEmitter(entity));

	// should not alter state
	playResult = soundHandler.PlayClip(entity, soundBuffer, Mix::AudioSpecification(false, 50, 1, 0, 0, 0, 5, 10), events, 0.0f);
	REQUIRE(playResult == false);
	REQUIRE(events.size() == 1);
	REQUIRE(soundHandler.HasEmitter(entity));
}

TEST_CASE("Repeated PauseClip calls", "[Sound]")
{
	Mix::SoundHandler soundHandler;
	std::set<Mix::AudioEventData> events;
	Mix::SoundBuffer soundBuffer;
	soundBuffer.OpenFromFile("Clips/Pew.wav");

	uint64_t entity = 12345;

	soundHandler.PlayClip(entity, soundBuffer, Mix::AudioSpecification(false, 50, 1, 0, 0, 0, 5, 10), events, 0.0f);

	bool pauseResult = soundHandler.PauseClip(entity, events);
	REQUIRE(pauseResult == true);
	REQUIRE(events.size() == 0);
	REQUIRE(soundHandler.HasEmitter(entity));

	// should not alter state
	pauseResult = soundHandler.PauseClip(entity, events);
	REQUIRE(pauseResult == false);
	REQUIRE(events.size() == 0);
	REQUIRE(soundHandler.HasEmitter(entity));
}

TEST_CASE("Repeated UnPauseClip calls", "[Sound]")
{
	Mix::SoundHandler soundHandler;
	std::set<Mix::AudioEventData> events;
	Mix::SoundBuffer soundBuffer;
	soundBuffer.OpenFromFile("Clips/Pew.wav");

	uint64_t entity = 12345;

	soundHandler.PlayClip(entity, soundBuffer, Mix::AudioSpecification(false, 50, 1, 0, 0, 0, 5, 10), events, 0.0f);

	// pause before un-pausing
	soundHandler.PauseClip(entity, events);

	// first un-pause
	bool unpauseResult = soundHandler.UnPauseClip(entity, events, 0.0f);
	REQUIRE(unpauseResult == true);
	REQUIRE(events.size() == 1);
	REQUIRE(soundHandler.HasEmitter(entity));

	// should not alter state
	unpauseResult = soundHandler.UnPauseClip(entity, events, 0.0f);
	REQUIRE(unpauseResult == false);
	REQUIRE(events.size() == 1);
	REQUIRE(soundHandler.HasEmitter(entity));
}

TEST_CASE("Pause invalid handling", "[Sound]")
{
	Mix::SoundHandler soundHandler;
	std::set<Mix::AudioEventData> events;
	uint64_t entity = 12345;

	{
		Mix::SoundBuffer soundBuffer;
		soundBuffer.OpenFromFile("Clips/Pew.wav");
		soundHandler.PlayClip(entity, soundBuffer, Mix::AudioSpecification(false, 50, 1, 0, 0, 0, 5, 10), events, 0.0f);
	}

	bool pauseResult = soundHandler.PauseClip(entity, events);
	REQUIRE(pauseResult == false);
	REQUIRE(events.size() == 0);
	REQUIRE(!soundHandler.HasEmitter(entity));
}

TEST_CASE("Un-pause invalid handling", "[Sound]")
{
	Mix::SoundHandler soundHandler;
	std::set<Mix::AudioEventData> events;
	uint64_t entity = 12345;

	{
		Mix::SoundBuffer soundBuffer;
		soundBuffer.OpenFromFile("Clips/Pew.wav");
		soundHandler.PlayClip(entity, soundBuffer, Mix::AudioSpecification(false, 50, 1, 0, 0, 0, 5, 10), events, 0.0f);
	}

	bool unpauseResult = soundHandler.UnPauseClip(entity, events, 0.0f);
	REQUIRE(unpauseResult == false);
	REQUIRE(events.size() == 0);
	REQUIRE(!soundHandler.HasEmitter(entity));
}

TEST_CASE("Stop invalid handling", "[Sound]")
{
	Mix::SoundHandler soundHandler;
	std::set<Mix::AudioEventData> events;
	uint64_t entity = 12345;

	{
		Mix::SoundBuffer soundBuffer;
		soundBuffer.OpenFromFile("Clips/Pew.wav");
		soundHandler.PlayClip(entity, soundBuffer, Mix::AudioSpecification(false, 50, 1, 0, 0, 0, 5, 10), events, 0.0f);
	}

	soundHandler.StopClip(entity, events, [&](uint64_t e)
	{
		REQUIRE(entity == e);
	});

	REQUIRE(events.size() == 0);
	REQUIRE(!soundHandler.HasEmitter(entity));
}
