#include "GameController.h"
#include "GameStateController.h"
#include "ObstacleUtils.h"
#include <iostream>
#include <random>

GameController::GameController(GameStateController* controller)
{
	stateController = controller;
	device = controller->device;
	soundEngine = controller->soundEngine;
	open = false;
	smgr = 0;
	guienv = 0;
	driver = 0;
	then = 0;
	bWorld = 0;
	gameConfig.loadConfig("cfg/gameconfig.gdat");
	gameConfig.saveConfig("cfg/gameconfig.gdat");
}

void GameController::update()
{
	u32 now = device->getTimer()->getTime();
	f32 delta = (f32)(now - then) / 1000.f;
	if (delta > .25) { //If the delta is too big, it's .25.
		delta = .25;
	}
	then = now;
	accumulator += delta;
	while (accumulator >= dt) {
		sceneECS.update(dt, delta); //in-game logic and physics
		t += dt;
		accumulator -= dt;
	}
	if (checkRunningScenario()) {
		stateController->setState(GAME_DEAD);
	}

	//interpolate leftover time?
	const f32 alpha = accumulator / dt;
}

void GameController::init()
{
	if (open) return;

	driver = device->getVideoDriver();
	smgr = device->getSceneManager();
	guienv = device->getGUIEnvironment();
	then = device->getTimer()->getTime();

	currentScenario = Scenario(SCENARIO_KILL_HOSTILES, 1, vector3df(0, 0, -50), vector3df(10, 20, 80));

	//bullet init
	broadPhase = new bt32BitAxisSweep3(btVector3(-100000, -100000, -100000), btVector3(100000, 100000, 100000));
	collisionConfig = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfig);
	solver = new btSequentialImpulseConstraintSolver();
	gPairCb = new btGhostPairCallback();
	bWorld = new BulletPhysicsWorld(dispatcher, broadPhase, solver, collisionConfig);
	bWorld->setGravity(btVector3(0, 0, 0));
	bWorld->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(gPairCb);
#if _DEBUG
	rend.setController(stateController);
	rend.setDebugMode(btIDebugDraw::DBG_DrawWireframe);
	bWorld->setDebugDrawer(&rend);
#endif 
	Scene scene;
	sceneECS = SceneManager(scene, this, bWorld); //Sets up the ECS scene

	collCb = new broadCallback();
	collCb->manager = &sceneECS;
	bWorld->getPairCache()->setOverlapFilterCallback(collCb);

	setDefaults(&sceneECS);
	open = true;
}

void GameController::close()
{
	if (!open) return;

	clearPlayerHUD();
	smgr->clear();
	delete broadPhase;
	bWorld->clearObjects();
	delete collisionConfig;
	delete dispatcher;
	delete solver;
	delete bWorld; //this likely leaks some memory
	delete collCb;
	delete gPairCb;
	//delete all the crap in the scenemanager too
	for (ComponentPool* pool : sceneECS.scene.componentPools) {
		delete pool; //pool's closed
	}
	open = false;
}

void GameController::clearPlayerHUD()
{
	for (auto id : SceneView<PlayerComponent>(sceneECS.scene)) {
		auto player = sceneECS.scene.get<PlayerComponent>(id);
		for (HUDElement* hud : player->HUD) {
			delete hud;
		}
		player->rootHUD->remove();
	}
}

bool GameController::OnEvent(const SEvent& event)
{
	if (event.EventType == EET_KEY_INPUT_EVENT) {
		for(auto entityId : SceneView<InputComponent>(sceneECS.scene)) { //Passes key input to the input components
			InputComponent* input = sceneECS.scene.get<InputComponent>(entityId);
			input->keysDown[event.KeyInput.Key] = event.KeyInput.PressedDown;
			if(event.KeyInput.Key == KEY_KEY_Y && !input->keysDown[KEY_KEY_Y]) {
				input->mouseControlEnabled = !input->mouseControlEnabled;
			}
			if (event.KeyInput.Key == KEY_KEY_U && !input->keysDown[KEY_KEY_U]) {
				input->safetyOverride = !input->safetyOverride;
			}
		}
	}
	if (event.EventType == EET_MOUSE_INPUT_EVENT) {
		for(auto entityId: SceneView<InputComponent>(sceneECS.scene)) { //Passes mouse input to the input components
			InputComponent* input = sceneECS.scene.get<InputComponent>(entityId);
			switch(event.MouseInput.Event) {
			case EMIE_LMOUSE_PRESSED_DOWN:
				input->leftMouseDown = true;
				break;
			case EMIE_LMOUSE_LEFT_UP:
				input->leftMouseDown = false;
				break;
			case EMIE_RMOUSE_PRESSED_DOWN:
				input->rightMouseDown = true;
				break;
			case EMIE_RMOUSE_LEFT_UP:
				input->rightMouseDown = false;
				break;
			case EMIE_MOUSE_MOVED:
				input->mousePixPosition.X = event.MouseInput.X;
				input->mousePixPosition.Y = event.MouseInput.Y;
				input->mousePosition.X = (event.MouseInput.X - ((f32)driver->getScreenSize().Width * .5f)) / ((f32)driver->getScreenSize().Width * .5f);
				input->mousePosition.Y = (event.MouseInput.Y - ((f32)driver->getScreenSize().Height * .5f)) / ((f32)driver->getScreenSize().Height * .5f);
				break;
			default:
				break;
			}
		}

	}
	if (event.EventType == EET_GUI_EVENT) {
		//handle GUI events here, but there probably aren't any that the HUD itself doesn't handle
	}
	return false;
}

bool GameController::checkRunningScenario()
{
	for (u32 i = 0; i < currentScenario.objectiveCount; ++i) {
		if (currentScenario.objectives[i] != INVALID_ENTITY && !sceneECS.scene.entityInUse(currentScenario.objectives[i])) {
			currentScenario.objectives[i] = INVALID_ENTITY;
			--currentScenario.activeObjectiveCount;
		}
	}
	if (currentScenario.activeObjectiveCount == 0) return true;
	return false;
}

void GameController::initScenario()
{
	std::cout << "Loading scenario...\n";

	ISceneNode* n = smgr->addLightSceneNode(0, vector3df(0, 10000, 0),
		SColor(200, 255, 180, 180), 50000.f);
	n->setID(ID_IsNotSelectable);

	switch (currentScenario.type) {
	case SCENARIO_KILL_HOSTILES:
		killHostilesScenario();
		break;
	default:
		break;
	}

	device->getCursorControl()->setActiveIcon(ECI_CROSS);

	ITexture* sky = driver->getTexture("effects/starskybox.png");
	n = smgr->addSkyBoxSceneNode(sky, sky, sky, sky, sky, sky);
	n->setID(ID_IsNotSelectable);

	std::cout << "Scenario loaded. \n";
}

void GameController::killHostilesScenario()
{
	EntityId player = createPlayerShipFromLoadout(&sceneECS, currentScenario.playerStartPos);
	initializeNeutralFaction(&sceneECS, player);

	std::vector<vector3df> obstaclePositions;

	for (u32 i = 0; i < 1000; ++i) {
		vector3df pos = getPointInSphere(vector3df(0, 0, 0), 5000.f);
		obstaclePositions.push_back(pos);
	}
	std::cout << "\n Done. Culling obstacles... ";
	for (u32 i = 0; i < obstaclePositions.size(); ++i) {
		vector3df pos = obstaclePositions[i];
		f32 radius = 75.f;
		if (isPointInSphere(pos, currentScenario.playerStartPos, radius)) {
			obstaclePositions.erase(obstaclePositions.begin() + i);
			continue;
		}
		if (isPointInSphere(pos, currentScenario.enemyStartPos, radius)) {
			obstaclePositions.erase(obstaclePositions.begin() + i);
		}
	}
	std::cout << "Done. Obstacles remaining: " << obstaclePositions.size() << "\n Building obstacles... \n ";
	for (u32 i = 0; i < obstaclePositions.size(); ++i) {
		u32 scale = std::rand() % 100;
		f32 mass = (f32)scale / 5.f;
		EntityId rock = createAsteroid(&sceneECS, obstaclePositions[i], randomRotationVector(), vector3df(scale, scale, scale), mass);

	}

	std::cout << "\nDone. Building hostiles... ";
	for (u32 i = 0; i < currentScenario.objectiveCount; ++i) {
		vector3df pos = getPointInSphere(currentScenario.enemyStartPos, 25.f);
		EntityId enemy = createDefaultAIShip(&sceneECS, pos); //todo: create AI ship generator that pulls from loaded ships
		initializeHostileFaction(&sceneECS, enemy);
		initializeDefaultSensors(&sceneECS, enemy);
		initializeShipParticles(&sceneECS, enemy);
		currentScenario.objectives[i] = enemy;
	}

	auto cloud = createGasCloud(&sceneECS, currentScenario.enemyStartPos, vector3df(10, 10, 10));

	std::cout << "Done. \n";
	//let's get us some rocks to bump around
}