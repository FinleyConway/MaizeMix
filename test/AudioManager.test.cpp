#include <catch2/catch_test_macros.hpp>
#include <MaizeMix/MaizeMix.h>

Mix::AudioManager c_Manager;
Mix::AudioClip c_AudioClip;

TEST_CASE("Audio clip create")
{
	c_AudioClip = c_Manager.CreateClip("Clips/Pew.wav", false);

	REQUIRE(c_AudioClip.GetChannel() == 1); // mono clip
	REQUIRE(c_AudioClip.GetDuration() >= 0.53f); // based on audacity
	REQUIRE(c_AudioClip.GetFrequency() == 44100);
	REQUIRE(c_AudioClip.GetLoadState() == Mix::AudioClip::LoadState::Loaded);
	REQUIRE(c_AudioClip.IsLoadInBackground() == false);
	REQUIRE(c_AudioClip.GetSampleCount() == 23460);  // based on audacity
}

TEST_CASE("Audio clip destroy")
{
	c_Manager.DestroyClip(c_AudioClip);

	REQUIRE(c_AudioClip.GetChannel() == 0);
	REQUIRE(c_AudioClip.GetDuration() == 0.0f);
	REQUIRE(c_AudioClip.GetFrequency() == 0);
	REQUIRE(c_AudioClip.GetLoadState() == Mix::AudioClip::LoadState::Unloaded);
	REQUIRE(c_AudioClip.IsLoadInBackground() == false);
	REQUIRE(c_AudioClip.GetSampleCount() == 0);
}

TEST_CASE("Audio clip error")
{
	auto error = c_Manager.CreateClip("error test", false);

	REQUIRE(error.GetChannel() == 0);
	REQUIRE(error.GetDuration() == 0.0f);
	REQUIRE(error.GetFrequency() == 0);
	REQUIRE(error.GetLoadState() == Mix::AudioClip::LoadState::Failed);
	REQUIRE(error.IsLoadInBackground() == false);
	REQUIRE(error.GetSampleCount() == 0);
}