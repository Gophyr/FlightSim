#include "AudioSource.h"

AudioSource::AudioSource()
{
	alGetError();
	alGenSources(1, &source);
	alSourcef(source, AL_PITCH, m_pitch);
	alSourcef(source, AL_GAIN, m_gain);
	alSource3f(source, AL_POSITION, m_position[0], m_position[1], m_position[2]);
	alSource3f(source, AL_VELOCITY, m_velocity[0], m_velocity[1], m_velocity[2]);
	alSourcei(source, AL_LOOPING, m_loop);
	alSourcei(source, AL_BUFFER, buf);
}
AudioSource::~AudioSource()
{
	alGetError();
	alSourcei(source, AL_BUFFER, 0); //detach the buffer, if it exists
	alDeleteSources(1, &source); //get rid of the source
}
void AudioSource::play(const ALuint bufToPlay)
{
	if (buf != 0) stop();

	buf = bufToPlay;

	alGetError();
	alSourcei(source, AL_BUFFER, buf);
	alSourcef(source, AL_PITCH, m_pitch);
	alSourcef(source, AL_GAIN, m_gain);
	alSource3f(source, AL_POSITION, m_position[0], m_position[1], m_position[2]);
	alSource3f(source, AL_VELOCITY, m_velocity[0], m_velocity[1], m_velocity[2]);
	alSourcei(source, AL_LOOPING, m_loop);

	alSourcePlay(source);
}

void AudioSource::stop()
{
	buf = 0;
	alGetError();
	alSourceStop(source);
	alSourcei(source, AL_BUFFER, 0);
}

void AudioSource::setPos(const vector3df pos) {
	m_position[0] = pos.X;
	m_position[1] = pos.Y;
	m_position[2] = pos.Z;
	alSource3f(source, AL_POSITION, m_position[0], m_position[1], m_position[2]);
}
void AudioSource::setVel(const btVector3 vel) {
	m_velocity[0] = vel.x();
	m_velocity[1] = vel.y();
	m_velocity[2] = vel.z();
	alSource3f(source, AL_VELOCITY, m_velocity[0], m_velocity[1], m_velocity[2]);
}
void AudioSource::setPitch(const f32 pitch)
{
	m_pitch = pitch;
	alSourcef(source, AL_PITCH, m_pitch);

}
void AudioSource::setGain(const f32 gain)
{
	m_gain = gain;
	alSourcef(source, AL_GAIN, m_gain);
}
void AudioSource::setLoop(const bool loop)
{
	m_loop = loop;
	alSourcei(source, AL_LOOPING, m_loop);
}

bool AudioSource::isFinished()
{
	if (buf == 0) return true;

	alGetError();
	ALint state;
	alGetSourcei(source, AL_SOURCE_STATE, &state);
	if (state == AL_STOPPED) return true;

	return false;
}