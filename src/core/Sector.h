#pragma once
#ifndef SECTOR_H
#define SECTOR_H
#include "BaseHeader.h"
#include "Scenario.h"

const u32 NUM_SCENARIO_OPTIONS = 4;

enum SECTOR_TYPE
{
	SECTOR_DEBRIS = 0,
	SECTOR_ASTEROID = 1,
	SECTOR_GAS = 2,
	SECTOR_SUPPLY_DEPOT = 3,
	SECTOR_GAS_GIANT = 4,
	SECTOR_FLEET_GROUP = 5,
	SECTOR_FINALE = 6
};

const u32 MAX_ENCOUNTERS = 7;

class Sector
{
public:
	Sector(SECTOR_TYPE stype) : type(stype) {
	}
	SECTOR_TYPE getType() { return type; }
	bool advance(); //returns true if we're at the end of the sector
	Scenario getScenario(u32 pos);
private:
	bool moved = false;
	virtual void buildScenarios() = 0;
	u32 currentEncounter;

	std::vector<Scenario> scenarioOptions;
	Scenario* currentScenario;
	SECTOR_TYPE type;
};

#endif