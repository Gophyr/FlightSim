#include "ShipUpdateSystem.h"

void jetOn(IParticleEmitter* jet)
{
	jet->setMaxParticlesPerSecond(150);
	jet->setMinParticlesPerSecond(50);
}
void jetOff(IParticleEmitter* jet)
{
	jet->setMaxParticlesPerSecond(0);
	jet->setMinParticlesPerSecond(0);
}
void jetPairOn(IParticleEmitter* jet1, IParticleEmitter* jet2)
{
	jetOn(jet1);
	jetOn(jet2);
}

void jetPairOff(IParticleEmitter* jet1, IParticleEmitter* jet2)
{
	jetOff(jet1);
	jetOff(jet2);
}

void setPairDir(IParticleEmitter* jet1, IParticleEmitter* jet2, vector3df dir)
{
	jet1->setDirection(dir * .02f);
	jet2->setDirection(dir * .02f);
}

void afterburnerJetOn(IParticleEmitter* engine, ILightSceneNode* light)
{
	engine->setMaxParticlesPerSecond(500);
	engine->setMinParticlesPerSecond(450);
	light->setRadius(3.f);
}

void afterburnerJetOff(IParticleEmitter* engine, ILightSceneNode* light)
{
	engine->setMaxParticlesPerSecond(300);
	engine->setMinParticlesPerSecond(100);
	light->setRadius(1.3f);
}

bool velocitySafetyCheck(f32 linVelocity, ShipComponent* ship, btVector3 velDir, btVector3 thrustDir)
{
	if (linVelocity >= ship->linearMaxVelocity) {
		if (std::abs(velDir.angle(thrustDir)) * RADTODEG >= 90.f) {
			return true;
		}
		return ship->safetyOverride;
	}
	return true;
}

bool angularSafetyCheck(f32 angVelocity, ShipComponent* ship, btVector3 velDir, btVector3 thrustDir)
{
	if (angVelocity >= ship->angularMaxVelocity) {
		if (std::abs(velDir.angle(thrustDir)) * RADTODEG >= 90.f) {
			return true;
		}
		return ship->safetyOverride;
	}
	return true;
}

void shipUpdateSystem(Scene& scene, f32 dt)
{
	for (auto entityId : SceneView<ShipComponent, BulletRigidBodyComponent>(scene)) {
		auto ship = scene.get<ShipComponent>(entityId);
		auto irr = scene.get<IrrlichtComponent>(entityId);
		BulletRigidBodyComponent* rbc = scene.get<BulletRigidBodyComponent>(entityId);
		btRigidBody* body = &rbc->rigidBody;

		f32 linVel = body->getLinearVelocity().length();
		f32 angVel = body->getAngularVelocity().length();
		btVector3 linDir = body->getLinearVelocity();
		btVector3 angDir = body->getAngularVelocity();
		velocitySafeNormalize(linDir);
		velocitySafeNormalize(angDir);

		btVector3 torque(0, 0, 0);
		btVector3 force(0, 0, 0);

		//Ugly looking, but all these emitters need adjusting every time the ship moves
		IParticleEmitter* up1 = ship->upJetEmit[0]->getEmitter();
		IParticleEmitter* up2 = ship->upJetEmit[1]->getEmitter();
		IParticleEmitter* down1 = ship->downJetEmit[0]->getEmitter();
		IParticleEmitter* down2 = ship->downJetEmit[1]->getEmitter();
		IParticleEmitter* left1 = ship->leftJetEmit[0]->getEmitter();
		IParticleEmitter* left2 = ship->leftJetEmit[1]->getEmitter();
		IParticleEmitter* right1 = ship->rightJetEmit[0]->getEmitter();
		IParticleEmitter* right2 = ship->rightJetEmit[1]->getEmitter();
		IParticleEmitter* back1 = ship->reverseJetEmit[0]->getEmitter();
		IParticleEmitter* back2 = ship->reverseJetEmit[1]->getEmitter();
		IParticleEmitter* engine = ship->engineJetEmit->getEmitter();

		setPairDir(up1, up2, getNodeUp(irr->node));
		setPairDir(down1, down2, getNodeDown(irr->node));
		setPairDir(left1, left2, getNodeLeft(irr->node));
		setPairDir(right1, right2, getNodeRight(irr->node));
		setPairDir(back1, back2, getNodeForward(irr->node));
		engine->setDirection(getNodeBackward(irr->node) * .02f);

		jetPairOff(up1, up2);
		jetPairOff(down1, down2);
		jetPairOff(left1, left2);
		jetPairOff(right1, right2);
		jetPairOff(back1, back2);
		//jetOff(engine);
		if (ship->moves[SHIP_STRAFE_DOWN] && velocitySafetyCheck(linVel, ship, linDir, getRigidBodyDown(body))) {
			jetPairOn(up1, up2);
			force += getForceDown(body, ship);
		}
		if (ship->moves[SHIP_STRAFE_UP] && velocitySafetyCheck(linVel, ship, linDir, getRigidBodyUp(body))) {
			jetPairOn(down1, down2);
			force += getForceUp(body, ship);
		}
		if (ship->moves[SHIP_STRAFE_LEFT] && velocitySafetyCheck(linVel, ship, linDir, getRigidBodyLeft(body))) {
			jetPairOn(right1, right2);
			force += getForceLeft(body, ship);
		}
		if (ship->moves[SHIP_STRAFE_RIGHT] && velocitySafetyCheck(linVel, ship, linDir, getRigidBodyRight(body))) {
			jetPairOn(left1, left2);
			force += getForceRight(body, ship);
		}
		if (ship->moves[SHIP_THRUST_FORWARD] && velocitySafetyCheck(linVel, ship, linDir, getRigidBodyForward(body))) {
			force += getForceForward(body, ship);
		}
		if (ship->moves[SHIP_STRAFE_BACKWARD] && velocitySafetyCheck(linVel, ship, linDir, getRigidBodyBackward(body))) {
			jetPairOn(back1, back2);
			force += getForceBackward(body, ship);
		}

		bool aft = ship->moves[SHIP_AFTERBURNER];
		if ((aft && ship->afterburnerFuel > 0) && velocitySafetyCheck(linVel, ship, linDir, getRigidBodyForward(body))) {
			ship->afterburnerFuel -= dt * ship->afterburnerFuelEfficiency;
			force += getForceAfterburner(body, ship);
		}
		if (aft != ship->afterburnerOn && ship->afterburnerFuel > 0) {
			if (aft) afterburnerJetOn(ship->engineJetEmit->getEmitter(), ship->engineLight);
			else afterburnerJetOff(ship->engineJetEmit->getEmitter(), ship->engineLight);

			ship->afterburnerOn = aft;
		}
		if (ship->afterburnerFuel <= 0) {
			afterburnerJetOff(ship->engineJetEmit->getEmitter(), ship->engineLight);
		}

		f32 pitchSensitivity = 1.f;
		f32 yawSensitivity = 1.f;
		if (ship->curPitch > 0 || ship->curPitch < 0) pitchSensitivity = ship->curPitch * .5f;
		if (ship->curYaw > 0 || ship->curYaw < 0) pitchSensitivity = ship->curYaw * .5f;

		//Updates the ship's torque based on what it's currently trying to do
		if (ship->moves[SHIP_PITCH_UP] && angularSafetyCheck(angVel, ship, angDir, getRigidBodyLeft(body))) {
			jetPairOn(down2, up1);
			torque += getTorquePitchUp(body, ship) * pitchSensitivity;
		}
		if (ship->moves[SHIP_PITCH_DOWN] && angularSafetyCheck(angVel, ship, angDir, getRigidBodyRight(body))) {
			jetPairOn(down1, up2);
			torque += getTorquePitchDown(body, ship) * pitchSensitivity;
		}
		if (ship->moves[SHIP_YAW_LEFT] && angularSafetyCheck(angVel, ship, angDir, getRigidBodyDown(body))) {
			jetPairOn(right2, left1);
			torque += getTorqueYawLeft(body, ship) * yawSensitivity;
		}
		if (ship->moves[SHIP_YAW_RIGHT] && angularSafetyCheck(angVel, ship, angDir, getRigidBodyUp(body))) {
			jetPairOn(right1, left2);
			torque += getTorqueYawRight(body, ship) * yawSensitivity;
		}
		if (ship->moves[SHIP_ROLL_LEFT] && angularSafetyCheck(angVel, ship, angDir, getRigidBodyForward(body))) {
			torque += getTorqueRollLeft(body, ship);
		}
		if (ship->moves[SHIP_ROLL_RIGHT] && angularSafetyCheck(angVel, ship, angDir, getRigidBodyBackward(body))) {
			torque += getTorqueRollRight(body, ship);
		}

		if (ship->moves[SHIP_STOP_ROTATION]) {
			torque += getTorqueToStopAngularVelocity(body, ship);
		}
		if (ship->moves[SHIP_STOP_VELOCITY]) {
			force += getForceToStopLinearVelocity(body, ship);
		}

		//Zeroes out the controls
		for (u32 i = 0; i < SHIP_MAX_MOVEMENTS; ++i) {
			ship->moves[i] = false;
		}
		ship->curPitch = 0.f;
		ship->curYaw = 0.f;

		rbc->rigidBody.applyTorqueImpulse(torque * dt);
		rbc->rigidBody.applyCentralImpulse(force * dt);

		auto hp = scene.get<HealthComponent>(entityId);
		if (!hp) continue;

		if (linVel > ship->linearMaxVelocity + 1.f) {
			f32 over = linVel - ship->linearMaxVelocity;
			hp->health -= over * ship->velocityTolerance * dt;
		}
		if (angVel > ship->angularMaxVelocity + .2f) {
			f32 over = angVel - ship->angularMaxVelocity;
			hp->health -= over * ship->velocityTolerance * dt;
		}
	}
}