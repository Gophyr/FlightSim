#include "Sector.h"
#include "GameStateController.h"

bool Sector::advance()
{

}
Scenario Sector::getScenario(u32 pos)
{
	return scenarioOptions[pos];
}