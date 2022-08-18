#include "AudioDriver.h"
#include <iostream>

const std::string musicPath = "audio/music/";
const std::string menuSoundPath = "audio/menu/";
const std::string gameSoundPath = "audio/game/";

AudioDriver::AudioDriver()
{
	device = alcOpenDevice(nullptr);
	if (device) {
		context = alcCreateContext(device, nullptr);
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

	musicSource = new AudioSource;
	menuSource = new AudioSource;
	musicSource->setLoop(true);
}

void AudioDriver::playGameSound(AudioSource source, std::string fname)
{
	ALuint buf = 0;
	if (loadedGameSounds.find(fname) != loadedGameSounds.end()) {
		buf = loadedGameSounds.at(fname);
	} else {
		buf = gameSounds.loadAudio(gameSoundPath + fname);
		if (buf == 0) return;
		loadedGameSounds[fname] = buf;
	}
	//also needs to register the audio source...
	source.play(buf);
}
void AudioDriver::playMenuSound(std::string fname)
{
	ALuint buf = 0;
	if (loadedMenuSounds.find(fname) != loadedMenuSounds.end()) {
		buf = loadedMenuSounds.at(fname);
	}
	else {
		buf = menuSounds.loadAudio(menuSoundPath + fname);
		if (buf == 0) return;
		loadedMenuSounds[fname] = buf;
	}
	menuSource->play(buf);
}
void AudioDriver::playMusic(std::string fname)
{
	/*
	if (currentMusic != 0) {
		if (!menuSounds.removeAudio(currentMusic)) {
		}
	}
	*/
	auto music = menuSounds.loadAudio(musicPath + fname);
	currentMusic = music;
	musicSource->play(music);
}
void AudioDriver::cleanupGameSounds()
{
	gameSounds.removeAllAudio();
	loadedGameSounds.clear();
}

void AudioDriver::audioUpdate()
{
}
