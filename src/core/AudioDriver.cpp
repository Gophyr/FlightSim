#include "AudioDriver.h"
#include <iostream>
#include "IrrlichtComponent.h"
#include "BulletRigidBodyComponent.h"

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

	alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
	alSpeedOfSound(600.f); //This is space. I do what I want.
	alDopplerFactor(.5f);

	std::cout << "Opened audio device: " << name << std::endl;

	musicSource = new AudioSource;
	menuSource = new AudioSource;
	musicSource->setLoop(true);
}

void AudioDriver::playGameSound(flecs::entity ent, std::string fname)
{
	ALuint buf = 0;
	if (loadedGameSounds.find(fname) != loadedGameSounds.end()) {
		buf = loadedGameSounds.at(fname);
	} else {
		buf = gameSounds.loadAudio(gameSoundPath + fname);
		if (buf == 0) return;
		loadedGameSounds[fname] = buf;
	}
	//also needs to register the audio source
	_SoundInstance inst;
	inst.id = ent;
	inst.src = new AudioSource;
	auto irr = ent.get<IrrlichtComponent>();
	auto rbc = ent.get<BulletRigidBodyComponent>();
	if (irr) {
		inst.src->setPos(irr->node->getAbsolutePosition());
	}
	if (rbc) {
		inst.src->setVel(rbc->rigidBody->getLinearVelocity());
	}
	
	inst.src->setRefDist(10.f);
	inst.src->setMaxDist(500.f);

	inst.src->play(buf);
	curGameSounds.push_back(inst);
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
	if (currentMusic != 0) {
		musicSource->stop();
		menuSounds.removeAudio(currentMusic);
	}

	auto music = menuSounds.loadAudio(musicPath + fname);
	currentMusic = music;
	musicSource->play(music);
}
void AudioDriver::cleanupGameSounds()
{
	setListenerPosition(vector3df(0, 0, 0));

	for (auto inst : curGameSounds) {
		inst.src->stop();
		delete inst.src;
	}
	curGameSounds.clear();

	gameSounds.removeAllAudio();
	loadedGameSounds.clear();
}

void AudioDriver::setListenerPosition(vector3df pos, vector3df up, btVector3 vel)
{
	auto err = alGetError();
	alListener3f(AL_POSITION, pos.X, pos.Y, -pos.Z);
	alListener3f(AL_VELOCITY, vel.x(), vel.y(), -vel.z());
	alListener3f(AL_ORIENTATION, up.X, up.Y, -up.Z);
	musicSource->setPos(pos);
	musicSource->setVel(vel);
	menuSource->setPos(pos);
	menuSource->setVel(vel);
}
