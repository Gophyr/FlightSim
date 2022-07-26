#pragma once

#ifndef SHIPCOMPONENT_H
#define SHIPCOMPONENT_H
#include "BaseHeader.h"
#include "Shaders.h"
#include <vector>

/*
* The ship component holds a lot of information about what a ship is.
* It holds the amount of hardpoints, a list of positions for those hardpoints, the weapon ID for those hardpoints,
* and the thrusts for each way the ship can move.
* 
* The thrusts are self explanatory. curPitch and curYaw are used in combination with mouse movement, to represent
* where the mouse is on the screen.
* 
* The "moves" array holds which movements are currently triggered.
* 
* Finally, the vectors and emitters hold the particle effects for the different thrusts a ship can do.
*/
struct ShipComponent {
	u32 shipDataId;

	f32 afterburnerFuel=0;
	f32 maxAfterburnerFuel=0;
	f32 afterburnerFuelEfficiency=0;
	bool afterburnerOn;

	vector3df upJetPos[2];
	vector3df downJetPos[2];
	vector3df leftJetPos[2];
	vector3df rightJetPos[2];
	vector3df reverseJetPos[2];
	vector3df engineJetPos;
};

#endif