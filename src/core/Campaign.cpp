#include "Campaign.h"
#include "GameStateController.h"

void Campaign::addAmmo(u32 amt)
{
	ammunition += amt;
}
u32 Campaign::removeAmmo(u32 amt)
{
	ammunition -= amt;
	if (ammunition < 0) {
		ammunition = 0;
		u32 ret = amt + ammunition;
		return ret;
	}
	return amt;
}
void Campaign::addSupplies(f32 amt)
{
	supplies += amt;
}
f32 Campaign::removeSupplies(f32 amt)
{
	supplies -= amt;
	if (supplies < 0) {
		supplies = 0;
		f32 ret = amt + supplies;
		return ret;
	}
	return amt;
}

ShipInstance* Campaign::createNewShipInstance(bool templateShip)
{
	ShipInstance* ship = new ShipInstance;
	ship->ship = shipData[0]->shipComponent;
	ship->hp.health = 100.f;
	ship->hp.maxHealth = 100.f;
	ship->hards = shipData[0]->hardpointComponent;

	if (!templateShip) {
		++shipCount;
		ship->id = shipCount;
	}

	for (u32 i = 0; i < MAX_HARDPOINTS; ++i) {
		ship->weps[i] = nullptr;
	}
	ship->physWep = nullptr;

	return ship;
}

WeaponInstance* Campaign::createNewWeaponInstance(WeaponInfoComponent wep)
{
	++wepCount;
	WeaponInstance* ret = new WeaponInstance;
	ret->id = wepCount;
	ret->wep = wep;
	return ret;
}
ShipInstance* Campaign::buildStarterShip()
{
	auto inst = createNewShipInstance();
	inst->ship = shipData[0]->shipComponent;
	inst->hards = shipData[0]->hardpointComponent;
	inst->weps[0] = createNewWeaponInstance(weaponData[3]->weaponComponent);
	inst->weps[1] = createNewWeaponInstance(weaponData[3]->weaponComponent);
	inst->physWep = createNewWeaponInstance(physWeaponData[1]->weaponComponent);
	return inst;
}

bool Campaign::addShipInstanceToHangar(ShipInstance* inst)
{
	if (ships.find(inst->id) != ships.end()) {
		return false;
	}
	ships[inst->id] = inst;
	return true;
}

bool Campaign::addWeapon(WeaponInstance* inst)
{
	if (inst->wep.phys) {
		if (physWeapons.find(inst->id) != physWeapons.end()) {
			return false;
		}
		physWeapons[inst->id] = inst;
	}
	else {
		if (weapons.find(inst->id) != weapons.end()) {
			return false;
		}
		weapons[inst->id] = inst;
	}
	return true;
}
ShipInstance* Campaign::getShip(u32 id)
{
	if (ships.find(id) == ships.end()) {
		return nullptr;
	}
	return ships[id];
}
WingmanData* Campaign::getWingman(u32 id)
{
	if (wingmen.find(id) == wingmen.end()) {
		return nullptr;
	}
	return wingmen[id];
}

WeaponInstance* Campaign::getWeapon(u32 id)
{
	if (weapons.find(id) == weapons.end()) {
		return nullptr;
	}
	return weapons[id];
}
WeaponInstance* Campaign::getPhysWeapon(u32 id)
{
	if (physWeapons.find(id) == physWeapons.end()) {
		return nullptr;
	}
	return physWeapons[id];
}

void Campaign::newCampaign()
{

}
bool Campaign::saveCampaign(std::string fname)
{

}
bool Campaign::loadCampaign(std::string fname)
{

}