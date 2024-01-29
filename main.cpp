#include <iostream>

#include "AudioClip.h"

int main()
{
	AudioClip musicClip = AudioClip("/home/finley/GameShiz/Sounds/Pew.wav", true);

	std::cout << musicClip.GetChannels() << std::endl;
	std::cout << musicClip.GetDuration() << std::endl;
	std::cout << musicClip.GetFrequency() << std::endl;
	std::cout << (int)musicClip.GetLoadState() << std::endl;

	return 0;
}