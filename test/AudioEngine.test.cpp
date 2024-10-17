#include <catch2/catch_test_macros.hpp>
#include <MaizeMix.h>

TEST_CASE("Playing sound", "[AudioEngine]")
{
	Mix::AudioEngine engine;
	auto clip = engine.CreateClip("Clips/Pew.wav", false);
	auto spec = Mix::AudioSpecification(false, 50, 1, 0, 0, 0, 5, 10);
	uint64_t entity = 12345;

	REQUIRE(engine.PlayAudio(entity, clip, spec) == true);
	REQUIRE(engine.EmitterCount() == 1);
}

TEST_CASE("Playing stream", "[AudioEngine]")
{
	Mix::AudioEngine engine;
	auto clip = engine.CreateClip("Clips/Pew.wav", true);
	auto spec = Mix::AudioSpecification(false, 50, 1, 0, 0, 0, 5, 10);
	uint64_t entity = 12345;

	REQUIRE(engine.PlayAudio(entity, clip, spec) == true);
	REQUIRE(engine.EmitterCount() == 1);
}

TEST_CASE("Attempt playing sound", "[AudioEngine]")
{
	Mix::AudioEngine engine;
	auto clip = engine.CreateClip("Clips/Pew.wav", false);
	auto spec = Mix::AudioSpecification(false, 50, 1, 0, 0, 0, 5, 10);
	uint64_t entity = 12345;

	engine.RemoveClip(clip);

	REQUIRE(engine.PlayAudio(entity, clip, spec) == false);
	REQUIRE(engine.EmitterCount() == 0);
}

TEST_CASE("Attempt playing stream", "[AudioEngine]")
{
	Mix::AudioEngine engine;
	auto clip = engine.CreateClip("Clips/Pew.wav", true);
	auto spec = Mix::AudioSpecification(false, 50, 1, 0, 0, 0, 5, 10);
	uint64_t entity = 12345;

	engine.RemoveClip(clip);

	REQUIRE(engine.PlayAudio(entity, clip, spec) == false);
	REQUIRE(engine.EmitterCount() == 0);
}

TEST_CASE("Playing max emitters", "[AudioEngine]")
{
	Mix::AudioEngine engine;
	auto clip = engine.CreateClip("Clips/Pew.wav", false);
	auto spec = Mix::AudioSpecification(false, 0, 1, 0, 0, 0, 5, 10);

	for (uint8_t i = 0; i < 255; ++i)
	{
		REQUIRE(engine.PlayAudio(i, clip, spec) == true);
	}

	REQUIRE(engine.EmitterCount() == 255);
	REQUIRE(engine.PlayAudio(255, clip, spec) == false);
	REQUIRE(engine.EmitterCount() == 255);
	REQUIRE(engine.HasHitMaxAudioSources() == true);
}

TEST_CASE("Auto sound removal", "[AudioEngine]")
{
	Mix::AudioEngine engine;
	auto clip = engine.CreateClip("Clips/Pew.wav", false);
	auto spec = Mix::AudioSpecification(false, 0, 1, 0, 0, 0, 5, 10);
	uint64_t entity = 12345;

	engine.SetAudioFinishCallback([&](uint64_t entityID)
	{
		REQUIRE(entityID == entity);
	});

	engine.PlayAudio(entity, clip, spec);

	for (uint32_t i = 0; i < 255; ++i)
	{
		engine.Update(0.016667f);
	}

	REQUIRE(engine.EmitterCount() == 0);
}

TEST_CASE("Auto stream removal", "[AudioEngine]")
{
	Mix::AudioEngine engine;
	auto clip = engine.CreateClip("Clips/Pew.wav", true);
	auto spec = Mix::AudioSpecification(false, 0, 1, 0, 0, 0, 5, 10);
	uint64_t entity = 12345;

	engine.SetAudioFinishCallback([&](uint64_t entityID)
	{
		REQUIRE(entityID == entity);
	});

	engine.PlayAudio(entity, clip, spec);

	for (uint32_t i = 0; i < 255; ++i)
	{
		engine.Update(0.016667f);
	}

	REQUIRE(engine.EmitterCount() == 0);
}

TEST_CASE("Pausing sound", "[AudioEngine]")
{
	Mix::AudioEngine engine;
	auto clip = engine.CreateClip("Clips/Pew.wav", false);
	auto spec = Mix::AudioSpecification(false, 50, 1, 0, 0, 0, 5, 10);
	uint64_t entity = 12345;

	engine.PlayAudio(entity, clip, spec);

	REQUIRE(engine.PauseAudio(entity) == true);
	REQUIRE(engine.EmitterCount() == 0);
}

TEST_CASE("Un-Pausing sound", "[AudioEngine]")
{
	Mix::AudioEngine engine;
	auto clip = engine.CreateClip("Clips/Pew.wav", false);
	auto spec = Mix::AudioSpecification(false, 50, 1, 0, 0, 0, 5, 10);
	uint64_t entity = 12345;

	engine.PlayAudio(entity, clip, spec);
	engine.PauseAudio(entity);

	REQUIRE(engine.UnpauseAudio(entity) == true);
	REQUIRE(engine.EmitterCount() == 1);
}

TEST_CASE("Stopping sound", "[AudioEngine]")
{
	Mix::AudioEngine engine;
	auto clip = engine.CreateClip("Clips/Pew.wav", false);
	auto spec = Mix::AudioSpecification(false, 50, 1, 0, 0, 0, 5, 10);
	uint64_t entity = 12345;

	engine.SetAudioFinishCallback([&](uint64_t entityID)
	{
		REQUIRE(entityID == entity);
	});

	engine.PlayAudio(entity, clip, spec);

	REQUIRE(engine.StopAudio(entity) == true);
	REQUIRE(engine.EmitterCount() == 0);
}

TEST_CASE("Listener", "[AudioEngine]")
{
	Mix::AudioEngine engine;
	auto clip = engine.CreateClip("Clips/Pew.wav", false);
	auto spec = Mix::AudioSpecification(false, 50, 1, 0, 0, 0, 5, 10);
	uint64_t entity = 12345;

	engine.PlayAudio(entity, clip, spec);

	REQUIRE(engine.SetListenerPosition(0, 0, 0) == true);
	REQUIRE(engine.SetGlobalVolume(0) == true);

	engine.StopAudio(entity);

	REQUIRE(engine.SetListenerPosition(0, 0, 0) == false);
	REQUIRE(engine.SetGlobalVolume(0) == false);
}