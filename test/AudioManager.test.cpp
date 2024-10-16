#include <catch2/catch_test_macros.hpp>
#include <MaizeMix.h>

Mix::AudioManager g_Manager;
Mix::AudioClip g_AudioClip;

TEST_CASE("Audio clip create")
{
	g_AudioClip = g_Manager.CreateClip("Clips/Pew.wav", false);

	REQUIRE(g_AudioClip.GetChannel() == 1); // mono clip
	REQUIRE(g_AudioClip.GetDuration() >= 0.53f); // based on audacity
	REQUIRE(g_AudioClip.GetFrequency() == 44100);
	REQUIRE(g_AudioClip.GetLoadState() == Mix::AudioClip::LoadState::Loaded);
	REQUIRE(g_AudioClip.IsLoadInBackground() == false);
	REQUIRE(g_AudioClip.GetSampleCount() == 23460);  // based on audacity
	REQUIRE(g_AudioClip.IsValid() == true);
}

TEST_CASE("Audio clip destroy")
{
	g_Manager.DestroyClip(g_AudioClip);

	REQUIRE(g_AudioClip.GetChannel() == 0);
	REQUIRE(g_AudioClip.GetDuration() == 0.0f);
	REQUIRE(g_AudioClip.GetFrequency() == 0);
	REQUIRE(g_AudioClip.GetLoadState() == Mix::AudioClip::LoadState::Unloaded);
	REQUIRE(g_AudioClip.IsLoadInBackground() == false);
	REQUIRE(g_AudioClip.GetSampleCount() == 0);
	REQUIRE(g_AudioClip.IsValid() == false);
}

TEST_CASE("Audio clip error")
{
	auto error = g_Manager.CreateClip("error test", false);

	REQUIRE(error.GetChannel() == 0);
	REQUIRE(error.GetDuration() == 0.0f);
	REQUIRE(error.GetFrequency() == 0);
	REQUIRE(error.GetLoadState() == Mix::AudioClip::LoadState::Failed);
	REQUIRE(error.IsLoadInBackground() == false);
	REQUIRE(error.GetSampleCount() == 0);
	REQUIRE(g_AudioClip.IsValid() == false);
}