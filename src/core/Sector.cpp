#include "Sector.h"
#include "GameStateController.h"

bool Sector::advance()
{
	if (moved) return false;
	++encounterNum;
	buildScenarios();
	moved = true;
	if (encounterNum >= 7) return true;
	return false;
}

void Sector::finishScenario()
{
	auto playerShip = campaign->getWingman(0)->assignedShip;

	auto hp = gameController->getPlayer().get<HealthComponent>();
	auto ship = gameController->getPlayer().get<ShipComponent>();
	auto hards = gameController->getPlayer().get<HardpointComponent>();

	playerShip->hp = *hp;
	playerShip->ship = *ship;
	for (u32 i = 0; i < hards->hardpointCount; ++i) {
		auto wep = hards->weapons[i].get_mut<WeaponInfoComponent>();
		if (wep->clip < wep->maxClip) { //if the clip is partially spent just reload the damn thing
			wep->clip = wep->maxClip;
			if (wep->ammunition >= wep->maxClip) {
				wep->ammunition -= wep->maxClip;
			}
			else {
				wep->ammunition = 0;
			}
		}
		playerShip->weps[i]->wep = *wep;
	}
	moved = false;
}

void DebrisSector::buildScenarios()
{
	for (u32 i = 0; i < NUM_SCENARIO_OPTIONS; ++i) {

	}
}