#include "Campaign.h"
#include "GameStateController.h"

void Campaign::addAmmo(s32 amt)
{
	ammunition += amt;
}
s32 Campaign::removeAmmo(s32 amt)
{
	ammunition -= amt;
	if (ammunition < 0) {
		u32 ret = amt + ammunition;
		ammunition = 0;
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
		f32 ret = amt + supplies;
		supplies = 0;
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

WeaponInstance* Campaign::createNewWeaponInstance(WeaponInfoComponent wep, bool templateWep)
{
	if(wep.wepDataId != 0 && !templateWep) ++wepCount;
	WeaponInstance* ret = new WeaponInstance;
	if (wep.wepDataId == 0 || templateWep) {
		ret->id = 0;
	}
	else {
		ret->id = wepCount;
	}
	ret->wep = wep;
	return ret;
}

WeaponInstance* Campaign::createRandomWeaponInstance()
{
	u32 id = std::rand() % weaponData.size();
	if (id == 0) id += 1;
	return createNewWeaponInstance(weaponData[id]->weaponComponent);
}

void Campaign::m_buildShipInstanceFromData(ShipData* data, ShipInstance* inst)
{
	inst->ship = data->shipComponent;
	inst->hards = data->hardpointComponent;
}

ShipInstance* Campaign::buildStarterShip()
{
	auto inst = createNewShipInstance();
	m_buildShipInstanceFromData(shipData[0], inst);
	inst->weps[0] = createNewWeaponInstance(weaponData[3]->weaponComponent);
	inst->weps[1] = createNewWeaponInstance(weaponData[3]->weaponComponent);
	inst->physWep = createNewWeaponInstance(physWeaponData[1]->weaponComponent);
	return inst;
}

ShipInstance* Campaign::createRandomShipInstance()
{
	auto inst = createNewShipInstance();
	u32 shipnum = std::rand() % shipData.size();
	auto ship = shipData[shipnum];
	m_buildShipInstanceFromData(ship, inst);
	return inst;
}

bool Campaign::addShipInstanceToHangar(ShipInstance* inst)
{
	if (getShip(inst->id)) return false;
	m_ships.push_back(inst);
	return true;
}

bool Campaign::addWeapon(WeaponInstance* inst)
{
	if (inst->wep.phys) {
		if (getPhysWeapon(inst->id)) return false;
		m_physWeapons.push_back(inst);
	}
	else {
		if (getWeapon(inst->id)) return false;
		m_weapons.push_back(inst);
	}
	return true;
}

bool Campaign::removeShipInstance(ShipInstance* inst)
{
	auto it = m_ships.begin();
	while (it != m_ships.end()) {
		if (inst->id == (*it)->id) {
			it = m_ships.erase(it);
			return true;
		}
		++it;
	}
	return false;
}
bool Campaign::removeWeapon(WeaponInstance* inst)
{
	if (inst->wep.phys) {
		auto it = m_physWeapons.begin();
		while (it != m_physWeapons.end()) {
			if (inst->id == (*it)->id) {
				it = m_physWeapons.erase(it);
				return true;
			}
			++it;
		}
	}
	else {
		auto it = m_weapons.begin();
		while (it != m_weapons.end()) {
			if (inst->id == (*it)->id) {
				it = m_weapons.erase(it);
				return true;
			}
			++it;
		}
	}
	return false;
}

ShipInstance* Campaign::getShip(u32 id)
{
	for (auto val : m_ships) {
		if (val->id == id) return val;
	}
	return nullptr;
}
WingmanData* Campaign::getWingman(u32 id)
{
	for (auto val : m_wingmen) {
		if (val->id == id) return val;
	}
	return nullptr;
}

WeaponInstance* Campaign::getWeapon(u32 id)
{
	for (auto val : m_weapons) {
		if (val->id == id) return val;
	}
	return nullptr;
}
WeaponInstance* Campaign::getPhysWeapon(u32 id)
{
	for (auto val : m_physWeapons) {
		if (val->id == id) return val;
	}
	return nullptr;
}
WingmanData* Campaign::getAssignedWingman(u32 pos)
{
	return assignedWingmen[pos];
}
void Campaign::setAssignedWingman(WingmanData* man, u32 pos)
{
	assignedWingmen[pos] = man;
	man->assigned = true;
}
void Campaign::removeAssignedWingman(u32 pos)
{
	if (assignedWingmen[pos]) {
		assignedWingmen[pos]->assignedShip = nullptr;
		assignedWingmen[pos]->assigned = false;
	}
	assignedWingmen[pos] = nullptr;
}
ShipInstance* Campaign::getAssignedShip(u32 pos)
{
	return assignedShips[pos];
}
void Campaign::setAssignedShip(ShipInstance* inst, u32 pos)
{
	assignedShips[pos] = inst;
}
void Campaign::removeAssignedShip(u32 pos)
{
	if (assignedShips[pos]) {
		assignedShips[pos]->inUseBy = nullptr;
	}
	assignedShips[pos] = nullptr;
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
	for (auto val : m_wingmen) {
		if (val) delete val;
	}
	for (auto val : m_ships) {
		if (val) delete val;
	}
	for (auto val : m_weapons) {
		if (val) delete val;
	}
	for (auto val : m_physWeapons) {
		if (val) delete val;
	}
	if (currentSector) delete currentSector;

	m_wingmen.clear();
	m_ships.clear();
	m_weapons.clear();
	m_physWeapons.clear();
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

	currentSector->buildScenarios();

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
		m_wingmen.push_back(data);
	}

	player = getWingman(0);
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