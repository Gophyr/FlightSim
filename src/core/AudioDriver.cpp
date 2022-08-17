#include "AudioDriver.h"
#include <fstream>
#include <iostream>

ALuint loadOgg(const char* path)
{
	ALenum error = 0;
	ALuint sound = 0;
	FILE* fp = 0;
	OggVorbis_File vf;
	vorbis_info* vi = 0;
	ALenum format = 0;
	short* pcmout = 0;

	fp = fopen(path, "rb");
	if (!fp) {
		std::cerr << "Could not open file: " << path << std::endl;
		return 0;
	}
	alGenBuffers(1, &sound);
	error = alGetError();
	if (error != AL_NO_ERROR) {
		std::cerr << "Error creating buffer: " << path << std::endl;
		fclose(fp);
		ov_clear(&vf);
		free(pcmout);
		return 0;
	}

	if (ov_open_callbacks(fp, &vf, NULL, 0, OV_CALLBACKS_NOCLOSE) < 0) {
		std::cerr << "Stream is not a valid OggVorbis stream.\n";
		fclose(fp);
		ov_clear(&vf);
		free(pcmout);
		return 0;
	}

	vi = ov_info(&vf, -1);
	format = vi->channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

	size_t dataLength = ov_pcm_total(&vf, -1) * vi->channels * 2;
	pcmout = (short*)malloc(dataLength);
	if (pcmout == 0) {
		std::cerr << "Out of memory. What the hell are you doing?\n";
		fclose(fp);
		ov_clear(&vf);
		free(pcmout);
		return 0;
	}
	for (size_t size = 0, offset = 0, sel = 0;
		(size = ov_read(&vf, (char*)pcmout + offset, 4096, 0, 2, 1, (int*)&sel)) != 0;
		offset += size) {
		if (size < 0) {
			std::cerr << "This ogg file is faulty.\n";
		}
	}
	alBufferData(sound, format, pcmout, dataLength, vi->rate);
	error = alGetError();
	if (error != AL_NO_ERROR) {
		std::cerr << "Failed to send audio info to OpenAL.\n";
		fclose(fp);
		ov_clear(&vf);
		free(pcmout);
		return 0;
	}

	fclose(fp);
	ov_clear(&vf);
	free(pcmout);

	return sound;
}

ALuint AudioBuffer::loadAudio(std::string fname)
{
	ALuint sound = loadOgg(fname.c_str());
	if (sound == 0) {
		std::cerr << "Error loading on " << fname << "!\n";
		return sound;
	}
	std::cout << "Loaded " << fname << std::endl;
	buffers.push_back(sound);
	return sound;
}
bool AudioBuffer::removeAudio(const ALuint& buf)
{
	auto it = buffers.begin();
	while (it != buffers.end()) {
		if (*it == buf) {
			alDeleteBuffers(1, &*it);
			it = buffers.erase(it);
			return true;
		}
		++it;
	}
	return false;
}
void AudioBuffer::removeAllAudio()
{
	for (auto buf : buffers) {
		alDeleteBuffers(1, &buf);
	}
}

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

	/*
	ALint state = AL_PLAYING;
	while (state == AL_PLAYING && alGetError() == AL_NO_ERROR)
	{
		alGetSourcei(source, AL_SOURCE_STATE, &state);
	}
	*/
}

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

void AudioDriver::audioUpdate()
{

}
