#pragma once
#ifndef AUDIOBUFFER_H
#define AUDIOBUFFER_H
#include "BaseHeader.h"

class AudioBuffer
{
public:
	ALuint loadAudio(std::string fname);
	bool removeAudio(const ALuint& buf);
	void removeAllAudio();
private:
	std::unordered_map<std::string, ALuint> buffers;
	//std::vector<ALuint> buffers;
};

#endif 