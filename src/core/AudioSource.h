#pragma once
#ifndef AUDIOSOURCE_H
#define AUDIOSOURCE_H
#include "BaseHeader.h"

class AudioSource
{
	public:
		AudioSource();
		~AudioSource();

		void play(const ALuint bufToPlay);
		void stop();

		void setPos(const vector3df pos);
		void setVel(const btVector3 vel);
		void setPitch(const f32 pitch);
		void setGain(const f32 gain);
		void setLoop(const bool loop);

		bool isFinished();
	private:
		f32 m_pitch = 1.f;
		f32 m_gain = 1.f;
		f32 m_position[3] = { 0,0,0 };
		f32 m_velocity[3] = { 0,0,0 };
		bool m_loop = false;
		ALuint source; //the identifier of the source, do not touch this
		//a source has exactly ONE attached buffer - this means that a source plays ONE sound.
		ALuint buf = 0;
};

#endif 