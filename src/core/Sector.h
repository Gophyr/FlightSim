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
		Sector(SECTOR_TYPE stype) : type(stype), moved(false), encounterNum(0) {
		}
		SECTOR_TYPE getType() { return type; }
		virtual bool advance(); //returns true if we're at the end of the sector
		bool sectorComplete() { return m_sectorComplete; }
		Scenario getCurrentScenario() { return *currentScenario; }
		Scenario getScenario(u32 pos) { return scenarioOptions[pos]; }
		void finishScenario();

		void selectCurrentScenario(u32 pos) { currentScenario = &scenarioOptions[pos]; }
		void deselectCurrentScenario() { currentScenario = nullptr; }
		u32 getEncounterNum() { return encounterNum; }

		bool hasMoved() { return moved; }

		virtual void buildScenarios() = 0;
		virtual void buildFinalScenario() = 0;
	protected:
		bool moved;
		bool m_sectorComplete;
		u32 encounterNum;

		Scenario scenarioOptions[NUM_SCENARIO_OPTIONS];
		Scenario* currentScenario;
		SECTOR_TYPE type;
};

class DebrisSector : public Sector
{
	public:
		DebrisSector() : Sector(SECTOR_DEBRIS) {}
		virtual void buildScenarios();
		virtual void buildFinalScenario();
	protected:
};

class AsteroidSector : public Sector
{
public:
	AsteroidSector() : Sector(SECTOR_ASTEROID) {}
	virtual void buildScenarios();
	virtual void buildFinalScenario();
protected:
};

#endif