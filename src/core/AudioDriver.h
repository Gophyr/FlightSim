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

		//void audioUpdate();

		//This plays a sound from the given source in the game and registers the source.
		void playGameSound(flecs::entity ent, std::string fname);
		//Plays a menu sound effect.
		void playMenuSound(std::string fname);
		//Plays music. Will halt any present music.
		void playMusic(std::string fname);
		
		void setListenerPosition(vector3df pos, btVector3 vel = btVector3(0,0,0));
		void cleanupGameSounds();
		struct _SoundInstance {
			flecs::entity id;
			AudioSource src;
		};
		std::list<_SoundInstance> curGameSounds;
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