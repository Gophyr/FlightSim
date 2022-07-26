#include "CarrierUtils.h"
#include "GameController.h"
#include "GameStateController.h"
#include "GuiController.h"
#include "HardpointComponent.h"

void initializeCarrier(flecs::entity id, f32 spawnRate, u32 reserveShips, vector3df scale)
{
	CarrierComponent carr;
	carr.reserveShips = reserveShips;
	carr.spawnRate = spawnRate;
	carr.scale = scale;
	carr.spawnTimer = 0;
	id.set<CarrierComponent>(carr);
}

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
	CarrierData* carr = stateController->carrierData[ship->shipDataId];
	auto irr = carrier.get_mut<IrrlichtComponent>();
	irr->name = carr->name;
	initializeShipCollisionBody(carrier, carrId, true);
	initializeHealth(carrier, carr->health);
	carrier.set<CarrierComponent>(carr->carrierComponent);
	//initializeCarrier(carrier, carr->carrierComponent.spawnRate, carr->carrierComponent.reserveShips, carr->carrierComponent.scale);

	ShipInstance* inst = newShipInstance();
	for (u32 i = 0; i < inst->hards.hardpointCount; ++i) {
		inst->weps[i] = stateController->weaponData[3]->weaponComponent;
	}

	auto carrcmp = carrier.get_mut<CarrierComponent>();
	carrcmp->shipTypeCount = 1;
	carrcmp->spawnShipTypes[0] = *inst;
	delete inst;

	initializePlayerFaction(carrier);
	initializeDefaultSensors(carrier);
	initializeDefaultAI(carrier);
	return carrier;
}

bool initializeTurretWeaponFromId(u32 id, flecs::entity turret, u32 hardpoint)
{
	if (id <= 0) {
		return false;
	}
	if (!turret.has<HardpointComponent>() || !turret.has<IrrlichtComponent>()) return false;

	auto turretIrr = turret.get<IrrlichtComponent>();

	auto hards = turret.get_mut<HardpointComponent>();
	auto wepEntity = game_world->entity().child_of(turret);
	loadWeapon(id, wepEntity, false);
	auto irr = wepEntity.get_mut<IrrlichtComponent>();
	irr->node->setParent(turretIrr->node);
	vector3df hpPos = hards->hardpoints[hardpoint];
	std::cout << "hpPos: " << hpPos.X << ", " << hpPos.Y << ", " << hpPos.Z << std::endl;
	irr->node->setPosition(hards->hardpoints[hardpoint]);
	hards->weapons[hardpoint] = wepEntity;

	irr->node->setScale(vector3df(.5f, .5f, .5f));

	return true;
}

flecs::entity createAlienCarrier(u32 carrId, vector3df pos, vector3df rot)
{
	auto carrier = createCarrierFromId(carrId, pos, rot);
	auto ship = carrier.get_mut<ShipComponent>();
	CarrierData* carr = stateController->carrierData[ship->shipDataId];
	auto irr = carrier.get_mut<IrrlichtComponent>();
	irr->name = carr->name;

	initializeShipCollisionBody(carrier, carrId, true);
	initializeHealth(carrier, carr->health);
	carrier.set<CarrierComponent>(carr->carrierComponent);
	//initializeCarrier(carrier, carr->carrierComponent.spawnRate, carr->carrierComponent.reserveShips, carr->carrierComponent.scale);

	ShipInstance* inst = newShipInstance();
	inst->ship = stateController->shipData[1]->shipComponent;
	for (u32 i = 0; i < inst->hards.hardpointCount; ++i) {
		inst->weps[i] = stateController->weaponData[1]->weaponComponent;
	}

	auto carrcmp = carrier.get_mut<CarrierComponent>();
	carrcmp->shipTypeCount = 1;
	carrcmp->spawnShipTypes[0] = *inst;
	delete inst;

	std::cout << "turret count: " << carrcmp->turretCount << "\n";
	for (u32 i = 0; i < carrcmp->turretCount; ++i) {
		std::cout << "Building turret on hardpoint " << i << "\n";
		vector3df pos = carrcmp->turretPositions[i] + irr->node->getPosition();
		carrcmp->turrets[i] = createTurretFromId(0, carrier, pos, carrcmp->turretRotations[i]);
		initializeHostileFaction(carrcmp->turrets[i]);
		initializeDefaultAI(carrcmp->turrets[i]);
		initializeDefaultSensors(carrcmp->turrets[i]);
		auto hards = carrcmp->turrets[i].get_mut<HardpointComponent>();
		for (u32 j = 0; j < hards->hardpointCount; ++j) {
			initializeTurretWeaponFromId(1, carrcmp->turrets[i], j);
		}
	}

	initializeHostileFaction(carrier);
	initializeDefaultSensors(carrier);
	initializeDefaultAI(carrier);
	return carrier;
}

void carrierDeathExplosionCallback(flecs::entity id)
{
	auto irr = id.get<IrrlichtComponent>();
	vector3df pos = irr->node->getAbsolutePosition();
	vector3df scale = irr->node->getScale();
	f32 avgscale = (scale.X + scale.Y + scale.Z);
	f32 rad = irr->node->getBoundingBox().getExtent().getLength() * 3 * avgscale;
	auto explosion = explode(pos, 3.f, avgscale, rad, 60.f, 600.f);
	gameController->registerSoundInstance(explosion, assets->getSoundAsset("carrierDeathExplosionSound"), 1.f, 100.f);
}