#include "AITypes.h"
#include "WeaponInfoComponent.h"
#include "BulletRigidBodyComponent.h"
#include "SensorComponent.h"
#include "IrrlichtComponent.h"
#include "ThrustComponent.h"
#include "IrrlichtUtils.h"
#include "HardpointComponent.h"
#include "ShipMovementUtils.h"

#include <iostream>

void setAIWeapon(flecs::entity wep, bool firing)
{
	if (!wep.is_alive()) return;
	if (!wep.has<WeaponInfoComponent>()) return;
	auto wepInfo = wep.get_mut<WeaponInfoComponent>();
	wepInfo->isFiring = firing;

}

void fireAtTarget(BulletRigidBodyComponent* rbc, const BulletRigidBodyComponent* targRBC, HardpointComponent* hards)
{
	if (!targRBC || !rbc) return;
	btVector3 facing = targRBC->rigidBody->getCenterOfMassPosition() - rbc->rigidBody->getCenterOfMassPosition();
	facing = facing.normalize();
	btVector3 forward = getRigidBodyForward(rbc->rigidBody);
	btScalar angle = forward.angle(facing);
	//if it's facing the ship, start shooting
	if ((angle * RADTODEG) < 30.f) { //converted to degrees so my overworked meat brain can better comprehend it
		for (unsigned int i = 0; i < hards->hardpointCount; ++i) {
			flecs::entity wep = hards->weapons[i];
			if (!wep.is_alive()) continue;
			if (!wep.has<WeaponInfoComponent>() || !wep.has<IrrlichtComponent>()) continue;
			auto wepInfo = wep.get_mut<WeaponInfoComponent>();
			auto irrComp = wep.get_mut<IrrlichtComponent>();
			wepInfo->isFiring = true;
			wepInfo->spawnPosition = irrComp->node->getAbsolutePosition() + (getNodeForward(irrComp->node) * 1.f);
			wepInfo->firingDirection = btVecToIrr(facing); //todo: the ai doesn't lead its shots at all, fix it
		}
	}
	else {
		for (unsigned int i = 0; i < hards->hardpointCount; ++i) {
			setAIWeapon(hards->weapons[i], false);
		}
	}
}

void DefaultShipAI::stateCheck(AIComponent* aiComp, SensorComponent* sensors, HealthComponent* hp)
{
	if (sensors->closestHostileContact == INVALID_ENTITY) {
		aiComp->state = AI_STATE_IDLE;
		return;
	}
	else if (hp->health <= (hp->maxHealth * aiComp->cowardice)) {
		//there's a hostile, but I'm low on health!
		aiComp->state = AI_STATE_FLEE; //aaaaieeeee!
		return;
	}
	//there's a hostile and I can take him!
	aiComp->state = AI_STATE_PURSUIT;
	//whoop its ass!
}

const bool m_distCheck(AIComponent* aiComp, BulletRigidBodyComponent* rbc, f32 m_dist)
{
	if (aiComp->wingCommander == INVALID_ENTITY || !aiComp->wingCommander.is_alive()) return true;
	auto cdrRBC = aiComp->wingCommander.get<BulletRigidBodyComponent>();
	if (!cdrRBC) return true;

	btVector3 dist = cdrRBC->rigidBody->getCenterOfMassPosition() - rbc->rigidBody->getCenterOfMassPosition();
	if (dist.length() > m_dist) return false;

	return true;
}

bool DefaultShipAI::distanceToWingCheck(AIComponent* aiComp, BulletRigidBodyComponent* rbc)
{
	return m_distCheck(aiComp, rbc, aiComp->wingDistance);
}
bool DefaultShipAI::combatDistanceToWingCheck(AIComponent* aiComp, BulletRigidBodyComponent* rbc)
{
	return m_distCheck(aiComp, rbc, aiComp->maxWingDistance);
}

void DefaultShipAI::idle(ThrustComponent* thrust, HardpointComponent* hards, BulletRigidBodyComponent* rbc)
{
	game_world->defer_suspend();
	//sit down and think about what you've done
	thrust->moves[STOP_ROTATION] = true;
	thrust->moves[STOP_VELOCITY] = true;

	for (unsigned int i = 0; i < hards->hardpointCount; ++i) {
		setAIWeapon(hards->weapons[i], false);
	}
	game_world->defer_resume();
}

void DefaultShipAI::flee(
	ThrustComponent* thrust, HardpointComponent* hards, BulletRigidBodyComponent* rbc, IrrlichtComponent* irr,
	flecs::entity fleeTarget)
{
	game_world->defer_suspend();
	if (fleeTarget == INVALID_ENTITY) return;
	if (!fleeTarget.has<IrrlichtComponent>()) return;
	auto fleeIrr = fleeTarget.get<IrrlichtComponent>();

	vector3df distance = fleeIrr->node->getPosition() - irr->node->getPosition(); //vector between the two things
	distance.normalize();
	vector3df targetVector = -distance; //runs away in the opposite direction

	//turn away and hit the gas as fast as possible
	smoothTurnToDirection(rbc->rigidBody, thrust, irrVecToBt(targetVector));
	thrust->moves[THRUST_FORWARD] = true;
	thrust->safetyOverride = true;

	for (unsigned int i = 0; i < hards->hardpointCount; ++i) {
		setAIWeapon(hards->weapons[i], false);
	}
	game_world->defer_resume();
}

void DefaultShipAI::pursue(
	ThrustComponent* thrust, HardpointComponent* hards, BulletRigidBodyComponent* rbc, IrrlichtComponent* irr, SensorComponent* sensors,
	flecs::entity pursuitTarget, f32 dt)
{
	game_world->defer_suspend();
	if (pursuitTarget == INVALID_ENTITY || !pursuitTarget.is_alive()) {
		sensors->targetContact = INVALID_ENTITY;
		return;
	}

	sensors->targetContact = pursuitTarget;

	if (!pursuitTarget.has<BulletRigidBodyComponent>()) return;
	auto targetRBC = pursuitTarget.get<BulletRigidBodyComponent>();

	btVector3 targetPos = targetRBC->rigidBody->getCenterOfMassPosition();
	btVector3 pos = rbc->rigidBody->getCenterOfMassPosition();

	btVector3 tailPos = targetPos + (getRigidBodyBackward(rbc->rigidBody) * 20.f);
	btVector3 dist = tailPos - pos;

	btVector3 facing = targetPos - pos;
	facing = facing.normalize();

	if (avoidObstacles(thrust, rbc, irr, dt)) return;

	//If it's not behind the ship, get behind it
	if (dist.length() > 30.f) {
		goToPoint(rbc->rigidBody, thrust, tailPos, dt);
	}
	else {
		//if it is behind it, start turning towards it
		thrust->moves[STOP_VELOCITY] = true;
		smoothTurnToDirection(rbc->rigidBody, thrust, facing);
	}
	fireAtTarget(rbc, targetRBC, hards);
	game_world->defer_resume();
}

void DefaultShipAI::pursueOnWing(
	ThrustComponent* thrust, AIComponent* aiComp, HardpointComponent* hards, BulletRigidBodyComponent* rbc, IrrlichtComponent* irr, SensorComponent* sensors,
	flecs::entity pursuitTarget, f32 dt)
{
	if (combatDistanceToWingCheck(aiComp, rbc)) {
		pursue(thrust, hards, rbc, irr, sensors, pursuitTarget, dt);
		return;
	}
	goToPoint(rbc->rigidBody, thrust, aiComp->wingCommander.get<BulletRigidBodyComponent>()->rigidBody->getCenterOfMassPosition(), dt);
	fireAtTarget(rbc, pursuitTarget.get<BulletRigidBodyComponent>(), hards);
}

void DefaultShipAI::formOnWing(
	ThrustComponent* thrust, AIComponent* aiComp, HardpointComponent* hards, BulletRigidBodyComponent* rbc,
	f32 dt)
{
	if (distanceToWingCheck(aiComp, rbc)) {
		idle(thrust, hards, rbc);
		return;
	}
	goToPoint(rbc->rigidBody, thrust, aiComp->wingCommander.get<BulletRigidBodyComponent>()->rigidBody->getCenterOfMassPosition(), dt);
}