#pragma once
#ifndef AUDIODRIVER_H
#define AUDIODRIVER_H
#include "BaseHeader.h"
#include "AudioBuffer.h"
#include "AudioSource.h"

class AudioDriver
{
	public:
		AudioDriver();

		void audioUpdate();

		//This plays a sound from the given source in the game and registers the source.
		void playGameSound(AudioSource source, std::string fname);
		//Plays a menu sound effect.
		void playMenuSound(std::string fname);
		//Plays music. Will halt any present music.
		void playMusic(std::string fname);
		
		void cleanupGameSounds();
	private:

		std::unordered_map<std::string, ALuint> loadedGameSounds;
		std::unordered_map<std::string, ALuint> loadedMenuSounds;
		AudioBuffer gameSounds;
		AudioBuffer menuSounds;
		ALuint currentMusic;

		AudioSource* musicSource; //should always be on top of the listener
		AudioSource* menuSource; //ditto - plays menu noises
		ALCcontext* context;
		ALCdevice* device;
};

#endif