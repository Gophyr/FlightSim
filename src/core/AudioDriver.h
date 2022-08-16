#pragma once
#ifndef AUDIODRIVER_H
#define AUDIODRIVER_H
#include "BaseHeader.h"

class AudioDriver
{
	public:
		AudioDriver() : context(nullptr), device(nullptr) {}
		void init();
		void audioUpdate();
	private:
		ALCcontext* context;
		ALCdevice* device;
};

#endif