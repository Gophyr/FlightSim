#pragma once
#ifndef AIUPDATESYSTEM_H
#define AIUPDATESYSTEM_H
#include "BaseHeader.h"
#include "AIComponent.h"
#include "IrrlichtComponent.h"
#include "BulletRigidBodyComponent.h"
#include "ShipComponent.h"
#include "SensorComponent.h"
#include "HealthComponent.h"
#include "ThrustComponent.h"
#include "HardpointComponent.h"

//Updates all AI components in the scene (allied, friendly, and neutral ships).
void AIUpdateSystem(flecs::iter it, 
	AIComponent* aic, IrrlichtComponent* irrc, BulletRigidBodyComponent* rbcs, ThrustComponent* thrc, HardpointComponent* hardsc, SensorComponent* sensc, HealthComponent* hpc);

#endif 