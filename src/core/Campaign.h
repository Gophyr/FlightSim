#pragma once
#ifndef CAMPAIGN_H
#define CAMPAIGN_H
#include "BaseHeader.h"
#include "Scenario.h"
#include "AIComponent.h"
#include "WeaponInfoComponent.h"
#include "LoadoutData.h"
#include "Sector.h"
#include <unordered_map>
#include <list>

/*
* The campaign holds all the data necessary to run the current campaign. The current difficulty acts as a multiplier on how many hostiles there are,
* and the current encounter holds which encounter is happening in the sector (max of four before a large scale battle). It also includes the data for
* how much damage can be repaired to ships and how much ammunition is available.
* 
* Presently, only about half of the data is used, but we're working on it, dammit.
*/

class Campaign
{
	public:
		Campaign() : currentDifficulty(1), ammunition(10), supplies(100.f) {
		}
		bool saveCampaign(std::string fname);
		bool loadCampaign(std::string fname);
		void newCampaign();
		Sector* getSector() {return currentSector;}

		//currently this leaks some memory and doesn't properly clean up the wingmen / ships

		void addAmmo(u32 amt);
		u32 removeAmmo(u32 amt);
		void addSupplies(f32 amt);
		f32 removeSupplies(f32 amt);

		ShipInstance* createNewShipInstance(bool templateShip = false);
		WeaponInstance* createNewWeaponInstance(WeaponInfoComponent wep); //agnostic on whether or not it's a physics weapon or not
		ShipInstance* buildStarterShip();
		bool addShipInstanceToHangar(ShipInstance* inst);
		bool addWeapon(WeaponInstance* inst);
		ShipInstance* getShip(u32 id);
		WingmanData* getWingman(u32 id);
		WeaponInstance* getWeapon(u32 id);
		WeaponInstance* getPhysWeapon(u32 id);

	private:
		Sector* currentSector;
		u32 currentDifficulty;
		u32 ammunition;
		f32 supplies;

		WingmanData* assignedWingmen[3];
		ShipInstance* assignedShips[3];
		WingmanData* player;
		std::unordered_map<u32, WingmanData*> wingmen;
		std::unordered_map<u32, ShipInstance*> ships;
		std::unordered_map<u32, WeaponInstance*> weapons;
		std::unordered_map<u32, WeaponInstance*> physWeapons;

		//all these start at 1 since we use 0 for template ids - that is, a template design for a carrier or something
		u32 shipCount = 0;
		u32 wepCount = 0;
};

#endif