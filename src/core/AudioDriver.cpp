#include "AudioDriver.h"

void AudioDriver::init()
{
	device = alcOpenDevice(NULL);
	if (device) {
		context = alcCreateContext(device, NULL);
		if (context) {
			alcMakeContextCurrent(context);
		}
	}
}

void AudioDriver::audioUpdate()
{

}
