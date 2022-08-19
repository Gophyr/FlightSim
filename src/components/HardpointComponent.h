#pragma once
#ifndef HARDPOINTCOMPONENT_H
#define HARDPOINTCOMPONENT_H
#include "BaseHeader.h"

const u32 MAX_HARDPOINTS = 6;
#define PHYS_HARDPOINT MAX_HARDPOINTS +1

struct HardpointComponent
{
	u32 hardpointCount;
	//This and the weapons array are initialized to the maximum number.
	vector3df hardpoints[MAX_HARDPOINTS];
	vector3df hardpointRotations[MAX_HARDPOINTS];
	flecs::entity weapons[MAX_HARDPOINTS];

	vector3df physWeaponHardpoint;
	vector3df physHardpointRot;
	flecs::entity physWeapon;
};

#endif 