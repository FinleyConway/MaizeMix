#include <catch2/catch_test_macros.hpp>
#include <MaizeMix.h>

TEST_CASE("Playing sound", "[AudioEngine]")
{
	Mix::AudioEngine engine;
	const auto clip = engine.CreateClip("Clips/Pew.wav", false);
	const auto spec = Mix::AudioSpecification(false, true, 100, 1);
	constexpr uint64_t entity = 12345;

	REQUIRE(engine.PlayAudio(entity, clip, spec) == true);
	REQUIRE(engine.EmitterCount() == 1);
}

TEST_CASE("Playing stream", "[AudioEngine]")
{
	Mix::AudioEngine engine;
	const auto clip = engine.CreateClip("Clips/Pew.wav", true);
	const auto spec = Mix::AudioSpecification(false, true, 100, 1);
	constexpr uint64_t entity = 12345;

	REQUIRE(engine.PlayAudio(entity, clip, spec) == true);
	REQUIRE(engine.EmitterCount() == 1);
}

TEST_CASE("Attempt playing sound", "[AudioEngine]")
{
	Mix::AudioEngine engine;
	auto clip = engine.CreateClip("Clips/Pew.wav", false);
	const auto spec = Mix::AudioSpecification(false, true, 100, 1);
	constexpr uint64_t entity = 12345;

	engine.RemoveClip(clip);

	REQUIRE(engine.PlayAudio(entity, clip, spec) == false);
	REQUIRE(engine.EmitterCount() == 0);
}

TEST_CASE("Attempt playing stream", "[AudioEngine]")
{
	Mix::AudioEngine engine;
	auto clip = engine.CreateClip("Clips/Pew.wav", true);
	const auto spec = Mix::AudioSpecification(false, true, 100, 1);
	constexpr uint64_t entity = 12345;

	engine.RemoveClip(clip);

	REQUIRE(engine.PlayAudio(entity, clip, spec) == false);
	REQUIRE(engine.EmitterCount() == 0);
}

TEST_CASE("Pause invalid handling", "[AudioEngine]")
{
	Mix::AudioEngine engine;
	auto clip = engine.CreateClip("Clips/Pew.wav", true);
	constexpr uint64_t entity = 12345;

	{
		engine.PlayAudio(entity, clip, Mix::AudioSpecification(false, true, 100, 1));
		engine.RemoveClip(clip);
	}

	REQUIRE(engine.PauseAudio(entity) == false);
	REQUIRE(engine.EmitterCount() == 0);
}

TEST_CASE("Un-pause invalid handling", "[AudioEngine]")
{
	Mix::AudioEngine engine;
	auto clip = engine.CreateClip("Clips/Pew.wav", true);
	constexpr uint64_t entity = 12345;

	{
		engine.PlayAudio(entity, clip, Mix::AudioSpecification(false, true, 100, 1));
		engine.RemoveClip(clip);
	}

	REQUIRE(engine.UnpauseAudio(entity) == false);
	REQUIRE(engine.EmitterCount() == 0);
}

TEST_CASE("Stop invalid handling", "[Sound]")
{
	Mix::AudioEngine engine;
	auto clip = engine.CreateClip("Clips/Pew.wav", true);
	constexpr uint64_t entity = 12345;

	engine.SetAudioFinishCallback([&](uint64_t e)
	{
		REQUIRE(e == entity);
	});

	{
		engine.PlayAudio(entity, clip, Mix::AudioSpecification(false, true, 100, 1));
		engine.RemoveClip(clip);
	}

	REQUIRE(engine.StopAudio(entity) == true);
	REQUIRE(engine.EmitterCount() == 0);
}

TEST_CASE("Playing max emitters", "[AudioEngine]")
{
	Mix::AudioEngine engine;
	auto clip = engine.CreateClip("Clips/Pew.wav", false);
	auto spec = Mix::AudioSpecification(false, true, 100, 1);

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
	const auto clip = engine.CreateClip("Clips/Pew.wav", false);
	const auto spec = Mix::AudioSpecification(false, true, 100, 1);
	constexpr uint64_t entity = 12345;

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
	const auto clip = engine.CreateClip("Clips/Pew.wav", true);
	const auto spec = Mix::AudioSpecification(false, true, 100, 1);
	constexpr uint64_t entity = 12345;

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
	const auto clip = engine.CreateClip("Clips/Pew.wav", false);
	const auto spec = Mix::AudioSpecification(false, true, 100, 1);
	constexpr uint64_t entity = 12345;

	engine.PlayAudio(entity, clip, spec);

	REQUIRE(engine.PauseAudio(entity) == true);
	REQUIRE(engine.EmitterCount() == 0);
}

TEST_CASE("Un-Pausing sound", "[AudioEngine]")
{
	Mix::AudioEngine engine;
	const auto clip = engine.CreateClip("Clips/Pew.wav", false);
	const auto spec = Mix::AudioSpecification(false, true, 100, 1);
	constexpr uint64_t entity = 12345;

	engine.PlayAudio(entity, clip, spec);
	engine.PauseAudio(entity);

	REQUIRE(engine.UnpauseAudio(entity) == true);
	REQUIRE(engine.EmitterCount() == 1);
}

TEST_CASE("Stopping sound", "[AudioEngine]")
{
	Mix::AudioEngine engine;
	const auto clip = engine.CreateClip("Clips/Pew.wav", false);
	const auto spec = Mix::AudioSpecification(false, true, 100, 1);
	constexpr uint64_t entity = 12345;

	engine.SetAudioFinishCallback([&](uint64_t entityID)
	{
		REQUIRE(entityID == entity);
	});

	engine.PlayAudio(entity, clip, spec);

	REQUIRE(engine.StopAudio(entity) == true);
	REQUIRE(engine.EmitterCount() == 0);
}

TEST_CASE("Play, pause, unpause, and stop", "[AudioEngine]")
{
	Mix::AudioEngine engine;
	const auto clip = engine.CreateClip("Clips/Pew.wav", false);
	constexpr uint64_t entity = 12345;

	engine.SetAudioFinishCallback([&](uint64_t e)
	{
		REQUIRE(entity == e);
	});

	// play the audio clip
	REQUIRE(engine.PlayAudio(entity, clip, Mix::AudioSpecification(false, true, 100, 1)) == true);
	REQUIRE(engine.EmitterCount() == 1);

	// pause the audio clip
	REQUIRE(engine.PauseAudio(entity) == true);
	REQUIRE(engine.EmitterCount() == 0);

	// un-pause the audio clip
	REQUIRE(engine.UnpauseAudio(entity) == true);
	REQUIRE(engine.EmitterCount() == 1);

	// stop the audio clip
	REQUIRE(engine.StopAudio(entity) == true);
	REQUIRE(engine.EmitterCount() == 0);
}

TEST_CASE("Repeated PlayClip calls", "[AudioEngine]")
{
	Mix::AudioEngine engine;
	const auto clip = engine.CreateClip("Clips/Pew.wav", false);
	constexpr uint64_t entity = 12345;

	REQUIRE(engine.PlayAudio(entity, clip, Mix::AudioSpecification(false, true, 100, 1)) == true);
	REQUIRE(engine.EmitterCount() == 1);

	// should not alter state
	REQUIRE(engine.PlayAudio(entity, clip, Mix::AudioSpecification(false, true, 100, 1)) == true);
	REQUIRE(engine.EmitterCount() == 1);
}

TEST_CASE("Repeated PauseClip calls", "[AudioEngine]")
{
	Mix::AudioEngine engine;
	const auto clip = engine.CreateClip("Clips/Pew.wav", false);
	constexpr uint64_t entity = 12345;

	engine.PlayAudio(entity, clip, Mix::AudioSpecification(false, true, 100, 1));

	REQUIRE(engine.PauseAudio(entity) == true);
	REQUIRE(engine.EmitterCount() == 0);

	// should not alter state
	REQUIRE(engine.PauseAudio(entity) == false);
	REQUIRE(engine.EmitterCount() == 0);
}

TEST_CASE("Repeated UnPauseClip calls", "[AudioEngine]")
{
	Mix::AudioEngine engine;
	const auto clip = engine.CreateClip("Clips/Pew.wav", false);
	constexpr uint64_t entity = 12345;

	engine.PlayAudio(entity, clip, Mix::AudioSpecification(false, true, 100, 1));

	// pause before un-pausing
	engine.PauseAudio(entity);

	// first un-pause
	REQUIRE(engine.UnpauseAudio(entity) == true);
	REQUIRE(engine.EmitterCount() == 1);

	// should not alter state
	REQUIRE(engine.UnpauseAudio(entity) == false);
	REQUIRE(engine.EmitterCount() == 1);
}

TEST_CASE("Listener", "[AudioEngine]")
{
	Mix::AudioEngine engine;
	const auto clip = engine.CreateClip("Clips/Pew.wav", false);
	const auto spec = Mix::AudioSpecification(false, true, 100, 1);
	constexpr uint64_t entity = 12345;

	engine.PlayAudio(entity, clip, spec);

	REQUIRE(engine.SetListenerPosition(0, 0, 0) == true);
	REQUIRE(engine.SetGlobalVolume(0) == true);

	engine.StopAudio(entity);

	REQUIRE(engine.SetListenerPosition(0, 0, 0) == false);
	REQUIRE(engine.SetGlobalVolume(0) == false);
}