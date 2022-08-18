#pragma once
#ifndef AUDIOSOURCE_H
#define AUDIOSOURCE_H
#include "BaseHeader.h"

class AudioSource
{
public:
	AudioSource();
	~AudioSource();
	ALuint source; //the identifier of the source, do not touch this
	void play(const ALuint bufToPlay);
	void stop(const ALuint bufToStop);

	f32 pitch = 1.f;
	f32 gain = 1.f;
	f32 position[3] = { 0,0,0 };
	f32 velocity[3] = { 0,0,0 };
	bool loop = false;

	//a source has exactly ONE attached buffer - this means that a source plays ONE sound.
	ALuint buf = 0;
};

#endif 