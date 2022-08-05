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

const u32 MAX_WINGMEN = 3;

class Campaign
{
	public:
		Campaign() : currentDifficulty(1), ammunition(10), supplies(100.f) {
		}
		bool saveCampaign(std::string fname);
		bool loadCampaign(std::string fname);
		void exitCampaign();
		void newCampaign();

		Sector* getSector() {return currentSector;}
		void returnToCampaign(); //returns from a given scenario

		void addAmmo(u32 amt);
		u32 removeAmmo(u32 amt);
		void addSupplies(f32 amt);
		f32 removeSupplies(f32 amt);

		ShipInstance* createNewShipInstance(bool templateShip = false);
		WeaponInstance* createNewWeaponInstance(WeaponInfoComponent wep); //agnostic on whether or not it's a physics weapon or not
		WeaponInstance* createRandomWeaponInstance(); //only regular weapons
		ShipInstance* buildStarterShip();

		bool addShipInstanceToHangar(ShipInstance* inst);
		bool addWeapon(WeaponInstance* inst);

		ShipInstance* getShip(u32 id);
		WingmanData* getWingman(u32 id);
		WeaponInstance* getWeapon(u32 id);
		WeaponInstance* getPhysWeapon(u32 id);

		WingmanData* getAssignedWingman(u32 pos);
		ShipInstance* getAssignedShip(u32 pos);

		bool assignWingmanToShip(WingmanData* wingman, ShipInstance* ship);

		u32 getDifficulty() { return currentDifficulty; }
	private:
		Sector* currentSector;
		u32 currentDifficulty;
		u32 ammunition;
		f32 supplies;

		WingmanData* assignedWingmen[MAX_WINGMEN];
		ShipInstance* assignedShips[MAX_WINGMEN];
		WingmanData* player;
		std::unordered_map<u32, WingmanData*> wingmen;
		std::unordered_map<u32, ShipInstance*> ships;
		std::unordered_map<u32, WeaponInstance*> weapons;
		std::unordered_map<u32, WeaponInstance*> physWeapons;

		u32 shipCount = 0;
		u32 wepCount = 0;
};

#endif