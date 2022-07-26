#include "AIUpdateSystem.h"
#include "AIComponent.h"
#include "AITypes.h"

void AIUpdateSystem(flecs::iter it, 
	AIComponent* aic, IrrlichtComponent* irrc, BulletRigidBodyComponent* rbcs, ThrustComponent* thrc, HardpointComponent* hardsc, SensorComponent* sensc, HealthComponent* hpc)
{
	for (auto i : it) {
		auto ai = &aic[i];
		auto hards = &hardsc[i];
		auto rbc = &rbcs[i];
		auto irr = &irrc[i];
		auto sensors = &sensc[i];
		auto hp = &hpc[i];
		auto thrust = &thrc[i];

		f32 dt = it.delta_time();
		ai->timeSinceLastStateCheck += dt;
		if (ai->timeSinceLastStateCheck >= ai->reactionSpeed) {
			ai->aiControls->stateCheck(ai, sensors, hp);
			ai->timeSinceLastStateCheck = 0;
		}
		switch (ai->state) {
			case AI_STATE_IDLE:
				ai->aiControls->formOnWing(thrust, ai, hards, rbc, dt);
				break;
			case AI_STATE_FLEE:
				ai->aiControls->flee(thrust, hards, rbc, irr, sensors->closestHostileContact);
				break;
			case AI_STATE_PURSUIT:
				ai->aiControls->pursueOnWing(thrust, ai, hards, rbc, irr, sensors, sensors->closestHostileContact, dt);
				break;
			default:
				break;
		}
	}
}
