#include "ScenarioUtils.h"
#include "GameStateController.h"
#include "GameController.h"
#include "LoadoutData.h"
#include "SceneManager.h"
#include "ShipUtils.h"

Scenario randomScenario(bool scramble)
{
	gvReader in;
	in.read("attributes/scenarios/scenariodesc.gdat");
	in.readLinesToValues();
	SCENARIO_TYPE type = static_cast<SCENARIO_TYPE>(std::rand() % SCENARIO_MAX_TYPES);
	if (type == SCENARIO_MAX_TYPES) type = SCENARIO_KILL_HOSTILES;
	SCENARIO_ENVIRONMENT env = static_cast<SCENARIO_ENVIRONMENT>(std::rand() % SCENENV_MAX_ENVIRONMENTS);
	if (env == SCENENV_MAX_ENVIRONMENTS) env = SCENENV_ASTEROID_FIELD;

	if (scramble) type = SCENARIO_SCRAMBLE;

	std::string location = scenarioEnvStrings.at(env);
	std::string description = in.values[location];
	description += "\n";
	description += in.values[scenarioStrings.at(type)];
	u32 objCount = std::rand() % (stateController->campaign.currentDifficulty * 3) + 1;
	if (objCount > SCENARIO_MAX_OBJECTIVES) objCount == SCENARIO_MAX_OBJECTIVES;

	if (scramble) objCount = 1; //being the single carrier needed to be taken out

	vector3df player(0, 0, -50);
	vector3df enemy(10, 20, 80);
	Scenario scen(type, env, objCount, player, enemy);

	in.clear();
	std::string path = "attributes/scenarios/environments/" + location + ".gdat";
	in.read(path);
	in.readLinesToValues();
	scen.detectionChance = in.getInt("detectionChance") + (1*std::rand() % stateController->campaign.currentDifficulty);
	if (scramble) scen.detectionChance = 0; //The man has already got you.
	scen.ammoRecovered = in.getInt("ammoRecovered") * (1 * std::rand() % stateController->campaign.currentDifficulty);
	scen.resourcesRecovered = in.getFloat("resourcesRecovered");
	scen.maxWepsRecovered = in.getInt("maxWeaponsRecovered");
	scen.maxShipsRecovered = in.getInt("maxShipsRecovered");

	in.clear();
	path = "attributes/scenarios/objectives/" + scenarioStrings.at(type) + ".gdat";
	in.read(path);
	in.readLinesToValues();
	scen.ammoRecovered += in.getUint("ammoRecovered") * (1 + std::rand() % stateController->campaign.currentDifficulty);
	scen.resourcesRecovered += in.getFloat("resourcesRecovered") * static_cast<f32>(std::rand() % stateController->campaign.currentDifficulty);
	scen.maxWepsRecovered += in.getInt("maxWeaponsRecovered");
	scen.maxShipsRecovered += in.getInt("maxShipsRecovered");

	if(env != SCENENV_EMPTY) setObstaclePositions(scen);

	scen.location = location;
	scen.description = description;
	return scen;
}

Scenario scrambleScenario() //for convenience
{
	return randomScenario(true);
}

void buildScenario(Scenario& scenario)
{
	ISceneNode* n = smgr->addLightSceneNode(0, vector3df(0, 10000, 0),
		SColor(200, 255, 180, 180), 50000.f);
	n->setID(ID_IsNotSelectable);

	EntityId player = createPlayerShipFromInstance(scenario.playerStartPos, vector3df(0, 0, 0));
	cullStartPosObstacleLocations(scenario);

	setScenarioType(scenario);
	setEnvironment(scenario);

	ITexture* sky = driver->getTexture("effects/starskybox.png");
	n = smgr->addSkyBoxSceneNode(sky, sky, sky, sky, sky, sky);
	n->setID(ID_IsNotSelectable);

}

void setEnvironment(Scenario& scenario)
{
	std::cout << "Setting environment... \n";
	switch (scenario.environment) {
	case SCENENV_ASTEROID_FIELD:
		buildAsteroidField(scenario);
		break;
	case SCENENV_GAS_FIELD:
		buildGasField(scenario);
		break;
	case SCENENV_DEBRIS_FIELD:
		buildDebrisField(scenario);
		break;
	default:
		std::cout << "No valid environment! \n";
		break;
	}
}

void setScenarioType(Scenario& scenario)
{
	std::cout << "Setting up scenario type... \n";
	switch (scenario.type) {
	case SCENARIO_KILL_HOSTILES:
		setKillHostilesScenario(scenario);
		break;
	case SCENARIO_SCRAMBLE:
		setScrambleScenario(scenario);
		break;
	case SCENARIO_DESTROY_OBJECT:
		setScrambleScenario(scenario); //the object in this case will currently be a carrier
	default:
		std::cout << "No valid type! Defaulting to kill hostiles. \n";
		setKillHostilesScenario(scenario);
		break;
	}
}

void setObstaclePositions(Scenario& scenario)
{
	std::cout << "Setting obstacle positions... ";
	for (u32 i = 0; i < 1000; ++i) {
		vector3df pos = getPointInSphere(vector3df(0, 0, 0), 5000.f);
		scenario.obstaclePositions.push_back(pos);
	}
	std::cout << "Done.\n";
}

void buildAsteroidField(Scenario& scenario) 
{
	std::cout << "Building asteroid field... ";
	for (u32 i = 0; i < scenario.obstaclePositions.size(); ++i) {
		u32 scale = std::rand() % 100;
		f32 mass = (f32)scale / 5.f;
		EntityId rock = createAsteroid(scenario.obstaclePositions[i], randomRotationVector(), vector3df(scale, scale, scale), mass);
	}
	std::cout << "Done.\n";
}

void buildGasField(Scenario& scenario) 
{
	std::cout << "Building gas field... ";
	for (u32 i = 0; i < scenario.obstaclePositions.size(); ++i) {
		u32 scale = std::rand() % 30;
		EntityId gas = createGasCloud(scenario.obstaclePositions[i], vector3df(scale, scale, scale));
	}
	std::cout << "Done. \n";
}

void buildDebrisField(Scenario& scenario)
{
	std::cout << "Building debris field... ";
	for (u32 i = 0; i < scenario.obstaclePositions.size(); ++i) {
		u32 scale = std::rand() % 70;
		f32 mass = (f32)scale / 5.f;
		EntityId debris = createDebris(scenario.obstaclePositions[i], randomRotationVector(), vector3df(scale, scale, scale), mass);
	}
	std::cout << "Done.\n";
}

void cullObstacleLocationsFromPosition(Scenario& scenario, vector3df pos, f32 rad)
{
	for (u32 i = 0; i < scenario.obstaclePositions.size(); ++i) {
		if (isPointInSphere(scenario.obstaclePositions[i], pos, rad)) {
			scenario.obstaclePositions.erase(scenario.obstaclePositions.begin() + i);
		}
	}
}

void cullStartPosObstacleLocations(Scenario& scenario)
{
	std::cout << "Culling obstacle positions... ";
	f32 rad = 75.f;
	cullObstacleLocationsFromPosition(scenario, scenario.playerStartPos, rad);
	cullObstacleLocationsFromPosition(scenario, scenario.enemyStartPos, rad);
	std::cout << "Done. Obstacles remaining: " << scenario.obstaclePositions.size() << std::endl;
}

void setKillHostilesScenario(Scenario& scenario)
{
	std::cout << "Setting up hostiles... ";
	for (u32 i = 0; i < scenario.objectiveCount; ++i) {
		vector3df pos = getPointInSphere(scenario.enemyStartPos, 25.f);
		EntityId enemy = createDefaultAIShip(pos, vector3df(0, 180, 0)); //todo: create AI ship generator that pulls from loaded ships
		auto obj = sceneManager->scene.assign<ObjectiveComponent>(enemy);
		obj->type = OBJ_DESTROY;
	}
	std::cout << "Done. \n";
}

void setScrambleScenario(Scenario& scenario)
{
	std::cout << "Setting up a scramble...";
	scenario.enemyStartPos.Z += 400;
	auto carr = createAlienCarrier(1, scenario.enemyStartPos, vector3df(0, 90, 0));
	auto obj = sceneManager->scene.assign<ObjectiveComponent>(carr);
	obj->type = OBJ_DESTROY;
	std::cout << "Done.\n";
}

bool collectObjective(EntityId id)
{
	auto irr = sceneManager->scene.get<IrrlichtComponent>(id);
	if (!irr) return true;
	auto playerirr = sceneManager->scene.get<IrrlichtComponent>(getPlayer());

	aabbox3df bb = irr->node->getBoundingBox();

	if (bb.intersectsWithBox(playerirr->node->getBoundingBox())) {
		destroyObject(id); //get rid of it, it's considered collected
		return true;
	}

	return false;
}

bool goToObjective(EntityId id)
{
	auto irr = sceneManager->scene.get<IrrlichtComponent>(id);
	if (!irr) return true;

	auto playerirr = sceneManager->scene.get<IrrlichtComponent>(getPlayer());

	vector3df dist = irr->node->getAbsolutePosition() - playerirr->node->getAbsolutePosition();
	if (dist.getLength() <= 10.f) return true;

	return false;
}

bool isObjectiveCompleted(EntityId id)
{
	//if there's no component / no entity available, either this function is unnecessary or the thing has been destroyed
	//in either case, return true and exit the premises as quickly as possible
	if (!sceneManager->scene.entityInUse(id)) return true;
	auto obj = sceneManager->scene.get<ObjectiveComponent>(id);
	if (!obj) return true;

	switch (obj->type) {
	case OBJ_DESTROY:
		return false; //if the entity is valid, clearly it hasn't been wrecked yet
	case OBJ_COLLECT:
		return collectObjective(id);
	case OBJ_GO_TO:
		return goToObjective(id);
	default:
		return false;
	}
}