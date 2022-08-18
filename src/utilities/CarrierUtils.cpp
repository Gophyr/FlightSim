#include "CarrierUtils.h"
#include "GameController.h"
#include "GameStateController.h"
#include "GuiController.h"
#include "HardpointComponent.h"

flecs::entity createCarrierFromId(u32 id, vector3df pos, vector3df rot)
{
	flecs::entity carrier = game_world->entity();
	loadShip(id, carrier, true);
	auto hards = carrier.get_mut<HardpointComponent>();
	auto irr = carrier.get_mut<IrrlichtComponent>();
	irr->node->setPosition(pos);
	irr->node->setRotation(rot);
	for (u32 i = 0; i < MAX_HARDPOINTS; ++i) {
		hards->weapons[i] = INVALID_ENTITY;
	}
	initializeShipParticles(carrier);
	gameController->registerDeathCallback(carrier, carrierDeathExplosionCallback);
	return carrier;
}

flecs::entity createTurretFromId(u32 id, flecs::entity owner, vector3df pos, vector3df startingRot)
{
	flecs::entity turret = game_world->entity().child_of(owner);
	turret.add<DoNotCollide>(owner);
	loadTurret(0, turret);
	auto irr = turret.get_mut<IrrlichtComponent>();
	auto shipIrr = owner.get<IrrlichtComponent>();
	//irr->node->setParent(shipIrr->node);
	irr->node->setPosition(pos);
	irr->node->setRotation(startingRot);
	irr->node->setScale(vector3df(5, 5, 5));
	auto sphere = new btSphereShape(irr->node->getScale().X);
	btVector3 scale(1.f, 1.f, 1.f);
	initBtRBC(turret, sphere, scale, .2f);

	auto shipRBC = owner.get_mut<BulletRigidBodyComponent>();
	auto rbc = turret.get_mut<BulletRigidBodyComponent>();


	initializeHealth(turret, 60.f);
	gameController->registerDeathCallback(turret, fighterDeathExplosionCallback);
	return turret;
}

flecs::entity createHumanCarrier(u32 carrId, vector3df pos, vector3df rot)
{
	auto carrier = createCarrierFromId(carrId, pos, rot);
	auto ship = carrier.get_mut<ShipComponent>();
	auto hards = carrier.get_mut<HardpointComponent>();
	CarrierData* carr = carrierData[ship->shipDataId];
	auto irr = carrier.get_mut<IrrlichtComponent>();
	irr->name = carr->name;
	initializeShipCollisionBody(carrier, carrId, true);
	initializeHealth(carrier, carr->health);
	carrier.set<CarrierComponent>(carr->carrierComponent);

	initializePlayerFaction(carrier);
	initializeDefaultSensors(carrier);
	initializeDefaultAI(carrier);
	return carrier;
}

flecs::entity createAlienCarrier(u32 carrId, vector3df pos, vector3df rot)
{
	auto carrier = createCarrierFromId(carrId, pos, rot);
	auto ship = carrier.get_mut<ShipComponent>();
	CarrierData* carr = carrierData[ship->shipDataId];
	auto irr = carrier.get_mut<IrrlichtComponent>();
	irr->name = carr->name;

	initializeShipCollisionBody(carrier, carrId, true);
	initializeHealth(carrier, carr->health);
	carrier.set<CarrierComponent>(carr->carrierComponent);

	ShipInstance* inst = campaign->createNewShipInstance(true);
	inst->ship = shipData[1]->shipComponent;
	inst->hards = shipData[1]->hardpointComponent;

	for (u32 i = 0; i < inst->hards.hardpointCount; ++i) {
		inst->weps[i] = campaign->createNewWeaponInstance(weaponData[1]->weaponComponent, true);
	}

	auto carrcmp = carrier.get_mut<CarrierComponent>();
	carrcmp->shipTypeCount = 1;
	carrcmp->spawnShipTypes[0] = *inst;
	delete inst;

	auto rbc = carrier.get_mut<BulletRigidBodyComponent>();

	for (u32 i = 0; i < carrcmp->turretCount; ++i) {
		vector3df pos = (carrcmp->turretPositions[i] * irr->node->getScale()) + irr->node->getPosition();

		carrcmp->turrets[i] = createTurretFromId(0, carrier, pos, carrcmp->turretRotations[i]);
		initializeHostileFaction(carrcmp->turrets[i]);
		initializeDefaultAI(carrcmp->turrets[i]);
		initializeDefaultSensors(carrcmp->turrets[i]);
		auto hards = carrcmp->turrets[i].get_mut<HardpointComponent>();
		for (u32 j = 0; j < hards->hardpointCount; ++j) {
			initializeWeaponFromId(1, carrcmp->turrets[i], j);
		}
		auto turrRBC = carrcmp->turrets[i].get_mut<BulletRigidBodyComponent>();
		
		btTransform trA, trB;
		trA.setIdentity();
		trB.setIdentity();
		trA.setOrigin(irrVecToBt(carrcmp->turretPositions[i] * irr->node->getScale()));
		trB.setOrigin(btVector3(0,0,0));
		auto constraint = new btGeneric6DofConstraint(*rbc->rigidBody, *turrRBC->rigidBody, trA, trB, false);
		carrcmp->turretConstraints[i] = constraint;
		constraint->setLinearLowerLimit(btVector3(0, 0, 0));
		constraint->setLinearUpperLimit(btVector3(0, 0, 0));
		constraint->setAngularLowerLimit(btVector3(-PI, -PI, -PI));
		constraint->setAngularUpperLimit(btVector3(PI, PI, PI));
		for (u32 j = 0; j < 6; ++j) {
			constraint->setParam(BT_CONSTRAINT_STOP_ERP, 1, j);
		}
		bWorld->addConstraint(constraint, true);
		
	}

	initializeHostileFaction(carrier);
	initializeDefaultSensors(carrier);
	initializeDefaultAI(carrier);
	return carrier;
}

void carrierDeathExplosionCallback(flecs::entity id)
{
	auto irr = id.get<IrrlichtComponent>();
	auto carr = id.get<CarrierComponent>();
	for (u32 i = 0; i < carr->turretCount; ++i) {
		carr->turrets[i].get_mut<HealthComponent>()->health = 0;
	}
	vector3df pos = irr->node->getAbsolutePosition();
	vector3df scale = irr->node->getScale();
	f32 avgscale = (scale.X + scale.Y + scale.Z);
	f32 rad = irr->node->getBoundingBox().getExtent().getLength() * 3 * avgscale;
	auto explosion = explode(pos, 3.f, avgscale, rad, 60.f, 600.f);
	audioDriver->playGameSound(explosion, "death_explosion_carrier.ogg");
}