#pragma once
#ifndef SECTOR_H
#define SECTOR_H
#include "BaseHeader.h"
#include "Scenario.h"

const u32 NUM_SCENARIO_OPTIONS = 4;
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

		virtual void buildScenarios();
		virtual void buildBossScenario();
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
	protected:
};

class AsteroidSector : public Sector
{
	public:
		AsteroidSector() : Sector(SECTOR_ASTEROID) {}
	protected:
};

#endif