#include "AudioSource.h"

AudioSource::AudioSource()
{
	alGenSources(1, &source);
	alSourcef(source, AL_PITCH, pitch);
	alSourcef(source, AL_GAIN, gain);
	alSource3f(source, AL_POSITION, position[0], position[1], position[2]);
	alSource3f(source, AL_VELOCITY, velocity[0], velocity[1], velocity[2]);
	alSourcei(source, AL_LOOPING, false);
	alSourcei(source, AL_BUFFER, buf);

}
AudioSource::~AudioSource()
{
	alDeleteSources(1, &source);
}
void AudioSource::play(const ALuint bufToPlay)
{
	buf = bufToPlay;
	alSourcei(source, AL_BUFFER, buf);
	alSourcePlay(source);
}

void AudioSource::stop(const ALuint bufToStop)
{
	buf = 0;
	alSourcei(source, AL_BUFFER, buf);
	alSourceStop(source);
}
