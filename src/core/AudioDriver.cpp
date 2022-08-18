#include "AudioDriver.h"
#include <iostream>

void AudioDriver::init()
{
	device = alcOpenDevice(NULL);
	if (device) {
		context = alcCreateContext(device, NULL);
		if (context) {
			alcMakeContextCurrent(context);
		}
	}
	const ALCchar* name = nullptr;
	if (alcIsExtensionPresent(device, "ALC_ENUMERATE_ALL_EXT"))
		name = alcGetString(device, ALC_ALL_DEVICES_SPECIFIER);
	if (!name || alcGetError(device) != AL_NO_ERROR)
		name = alcGetString(device, ALC_DEVICE_SPECIFIER);
	std::cout << "Opened audio device: " << name << std::endl;
}

void AudioDriver::playGameSound(std::string fname)
{

}
void AudioDriver::playMenuSound(std::string fname)
{

}
void AudioDriver::playMusic(std::string fname)
{
	auto music = menuSounds.loadAudio(fname);
}


void AudioDriver::audioUpdate()
{
}
