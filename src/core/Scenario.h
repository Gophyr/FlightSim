#pragma once
#ifndef SCENARIO_H
#define SCENARIO_H
#include "BaseHeader.h"
#include <unordered_map>

//Different types of scenario available.
enum SCENARIO_TYPE
{
	SCENARIO_KILL_HOSTILES,
	SCENARIO_DESTROY_OBJECT,
	SCENARIO_SALVAGE,
	SCENARIO_MAX_TYPES,
	SCENARIO_SCRAMBLE, // this is outside because we don't want this to generate when creating random scenarios
	SCENARIO_CARRIER_BATTLE, //ditto, this is a larger boss fight
	SCENARIO_NOT_LOADED
};

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

//Strings for loading a scenario from file.
const std::unordered_map<SCENARIO_TYPE, std::string> scenarioStrings = {
	{SCENARIO_KILL_HOSTILES, "killHostiles"},
	{SCENARIO_SALVAGE, "salvage"},
	{SCENARIO_DESTROY_OBJECT, "destroyObject"},
	{SCENARIO_SCRAMBLE, "scramble"},
	{SCENARIO_CARRIER_BATTLE, "carrierBattle"}
};

//Strings for loading and displaying a scenario environment.
const std::unordered_map<SECTOR_TYPE, std::string> scenarioEnvStrings = {
	{SECTOR_DEBRIS, "debris"},
	{SECTOR_ASTEROID, "asteroid"},
	{SECTOR_GAS, "gas"},
	{SECTOR_SUPPLY_DEPOT, "supply_depot"},
	{SECTOR_GAS_GIANT, "gas_giant"},
	{SECTOR_FLEET_GROUP, "fleet_group"},
	{SECTOR_FINALE, "final_battle"}
};

//Max amount of objectives possible (enemies to kill, things to blow up, etc).
const u32 SCENARIO_MAX_OBJECTIVES = 10;

/*
* Scenarios hold the information for... well, a scenario. They get randomly generated and include the objectives, player start,
* enemy start, environment, type, objective count, and others. When initialized (i.e. run by the actual game when the player hits start)
* the entities for the game get created at the positions listed here. The scenario will get constantly checked to see whether the objectives
* have been completed, and if so, the bool "complete" will be set to true and the scenario will end (either that or the player dies like a dog).
*/
struct Scenario
{
	Scenario() : complete(false)
	{ }
	Scenario(SCENARIO_TYPE type, SECTOR_TYPE env, u32 objCount, vector3df playerStart, vector3df enemyStart) :
		type(type), objectiveCount(objCount), playerStartPos(playerStart), enemyStartPos(enemyStart), complete(false),
		environment(env) { }
	SCENARIO_TYPE type;
	SECTOR_TYPE environment;
	u32 detectionChance;

	std::string location;
	std::string description;

	std::vector<vector3df> obstaclePositions;

	u32 objectiveCount;

	vector3df playerStartPos;
	vector3df enemyStartPos;

	bool complete;

	u32 ammoRecovered;
	f32 resourcesRecovered;
	u32 maxWepsRecovered;
	u32 maxShipsRecovered;

	bool detected() {
		u32 roll = std::rand() % 100;
		if (roll < detectionChance) return true;
		return false;
	}
};
#endif 