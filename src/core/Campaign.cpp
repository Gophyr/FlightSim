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
	if(wep.wepDataId != 0) ++wepCount;
	WeaponInstance* ret = new WeaponInstance;
	ret->id = wepCount;
	ret->wep = wep;
	return ret;
}

WeaponInstance* Campaign::createRandomWeaponInstance()
{
	u32 id = std::rand() % weaponData.size();
	if (id == 0) id += 1;
	return createNewWeaponInstance(weaponData[id]->weaponComponent);
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
WingmanData* Campaign::getAssignedWingman(u32 pos)
{
	return assignedWingmen[pos];
}
ShipInstance* Campaign::getAssignedShip(u32 pos)
{
	return assignedShips[pos];
}

bool Campaign::assignWingmanToShip(WingmanData* wingman, ShipInstance* ship)
{
	if (!wingman || !ship) return false;
	ship->inUseBy = wingman;
	wingman->assignedShip = ship;
	wingman->assigned = true;

	return true;
}

void Campaign::exitCampaign()
{
	std::cout << "Cleaning out old data... ";
	for (auto [id, val] : wingmen) {
		if (val) delete val;
	}
	for (auto [id, val] : ships) {
		if (val) delete val;
	}
	for (auto [id, val] : weapons) {
		if (val) delete val;
	}
	for (auto [id, val] : physWeapons) {
		if (val) delete val;
	}
	wingmen.clear();
	ships.clear();
	weapons.clear();
	physWeapons.clear();
	std::cout << "Done.\n";
}
void Campaign::newCampaign()
{
	std::cout << "Building new campaign... \n";
	exitCampaign();
	shipCount = 0;
	wepCount = 0;
	currentDifficulty = 1;
	ammunition = 10;
	supplies = 100.f;
	currentSector = new DebrisSector();

	auto playerShip = buildStarterShip();
	addShipInstanceToHangar(playerShip);
	addShipInstanceToHangar(buildStarterShip());
	addWeapon(createNewWeaponInstance(weaponData[0]->weaponComponent));
	addWeapon(createNewWeaponInstance(physWeaponData[0]->weaponComponent));

	std::cout << "Loading wingmen... \n";
	std::string wingmanPath = "attributes/wingmen/";
	for (const auto& file : std::filesystem::directory_iterator(wingmanPath)) {
		WingmanData* data = new WingmanData;
		if (!loadWingman(file.path().string(), *data)) continue;
		wingmen[data->id] = data;
	}

	player = wingmen[0];
	assignWingmanToShip(player, playerShip);

	for (u32 i = 0; i < MAX_WINGMEN; ++i) {
		assignedWingmen[i] = nullptr;
		assignedShips[i] = nullptr;
	}
	std::cout << "Done loading wingmen. \n";
	std::cout << "Done loading campaign. \n";
}
void Campaign::returnToCampaign()
{
	getSector()->finishScenario();
	++campaign->currentDifficulty;
}

bool Campaign::saveCampaign(std::string fname)
{
	return false;
}
bool Campaign::loadCampaign(std::string fname)
{
	return false;
}