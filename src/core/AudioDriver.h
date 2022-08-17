#pragma once
#ifndef AUDIODRIVER_H
#define AUDIODRIVER_H
#include "BaseHeader.h"

class AudioBuffer
{
public:
	ALuint loadAudio(std::string fname);
	bool removeAudio(const ALuint& buf);
	void removeAllAudio();
private:
	std::vector<ALuint> buffers;
};

class AudioSource
{
public:
	AudioSource();
	~AudioSource();
	ALuint source;
	void play(const ALuint bufToPlay);
	f32 pitch = 1.f;
	f32 gain = 1.f;
	f32 position[3] = { 0,0,0 };
	f32 velocity[3] = { 0,0,0 };
	bool loop = false;
	ALuint buf = 0;
};

class AudioDriver
{
	public:
		AudioDriver() : context(nullptr), device(nullptr) {}
		void init();
		void audioUpdate();

		AudioBuffer gameSounds;
	private:
		ALCcontext* context;
		ALCdevice* device;
};

#endif