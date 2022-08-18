#pragma once
#ifndef AUDIODRIVER_H
#define AUDIODRIVER_H
#include "BaseHeader.h"
#include "AudioBuffer.h"
#include "AudioSource.h"

class AudioDriver
{
	public:
		AudioDriver() : context(nullptr), device(nullptr) {}
		void init();
		void audioUpdate();

		void playGameSound(std::string fname);
		void playMenuSound(std::string fname);
		void playMusic(std::string fname); //will halt the current music
	private:
		ALuint currentMusic;
		AudioBuffer gameSounds;
		AudioBuffer menuSounds;
		AudioSource musicSource; //should always be on top of the listener
		ALCcontext* context;
		ALCdevice* device;
};

#endif