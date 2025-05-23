#include "GameStateController.h"

#include <iostream>

GameStateController::GameStateController(VideoConfig vconf)
{
	std::srand((u32)time(NULL));
	gameInitialized = false;
	videoConfig = vconf;
}

void GameStateController::init()
{
	std::cout << "Starting game... \n";
	device->setEventReceiver(stateController);
	guienv->setUserEventReceiver(stateController);
	then = device->getTimer()->getTime();
	state = GAME_MENUS; //Initial state
	driver->setMinHardwareBufferVertexCount(0);
	driver->setTextureCreationFlag(ETCF_ALWAYS_32_BIT, true);

	loadShipAndWeaponData();
	assets->setFilenames();

	audioDriver->playMusic("main_menu.ogg");

	gameController = new GameController;

	guiController = new GuiController;
	guiController->init();

	IGUIFont* defaultFont = assets->getFontAsset("defaultFont");
	if (defaultFont) {
		guienv->getSkin()->setFont(defaultFont);
	}
	IGUIFont* tooltipDefaultFont = assets->getFontAsset("defaultTooltipFont");
	if (tooltipDefaultFont) {
		guienv->getSkin()->setFont(tooltipDefaultFont, EGDF_TOOLTIP);
	}
	guienv->getSkin()->setColor(EGDC_BUTTON_TEXT, SColor(255, 140, 250, 255));

	//testing out XML reader...
	//DialogueTree testTree("dialogue/testdialogue.xml");
	//testTree.print();

	std::cout << "Game initialized!\n";
	gameInitialized = true;
}

void GameStateController::loadShipAndWeaponData()
{
	std::string basepath = "attributes/";
	std::string weaponpath = basepath + "weapons/";
	std::string shippath = basepath + "ships/";
	std::string hullpath = basepath + "hulls/";
	std::string obstpath = basepath + "obstacles/";
	std::string carrierpath = basepath + "carriers/";
	std::string turretpath = basepath + "turrets/";

	gvReader in;
	std::cout << "Loading ships... \n";
	for (const auto& file : std::filesystem::directory_iterator(shippath)) {
		u32 id = loadShipData(file.path().string(), in);
		if (id != -1) {
			btConvexHullShape hull;
			std::string fname = hullpath + shipData[id]->name + ".bullet";
			if (loadHull(fname, hull)) {
				std::cout << "Hull loaded for " << file.path().string() << ".\n";
				shipData[id]->collisionShape = hull;
			}
			else {
				std::cout << "Could not load hull for " << file.path().string() << ". Building hull... ";
				IMesh* mesh = smgr->getMesh(shipData[id]->shipMesh.c_str());
				shipData[id]->collisionShape = createCollisionShapeFromMesh(mesh);
				saveHull(fname, shipData[id]->collisionShape);
				smgr->getMeshCache()->removeMesh(mesh);
				std::cout << "Done. New hull saved to " << fname << ".\n";
			}
		}
		in.clear();
	}
	std::cout << "Done loading ships. \nLoading carriers... \n";
	for (const auto& file : std::filesystem::directory_iterator(carrierpath)) {
		u32 id = loadShipData(file.path().string(), in, true);
		if (id != -1) {
			btConvexHullShape hull;
			std::string fname = hullpath + carrierData[id]->name + ".bullet";
			if (loadHull(fname, hull)) {
				std::cout << "Hull loaded for " << file.path().string() << ".\n";
				carrierData[id]->collisionShape = hull;
			}
			else {
				std::cout << "Could not load hull for " << file.path().string() << ". Building hull... ";
				IMesh* mesh = smgr->getMesh(carrierData[id]->shipMesh.c_str());
				carrierData[id]->collisionShape = createCollisionShapeFromMesh(mesh);
				saveHull(fname, carrierData[id]->collisionShape);
				smgr->getMeshCache()->removeMesh(mesh);
				std::cout << "Done. New hull saved to " << fname << ".\n";
			}
		}
	}
	std::cout << "Done loading carriers. \nLoading turrets... \n";
	for (const auto& file : std::filesystem::directory_iterator(turretpath)) {
		loadTurretData(file.path().string(), in);
		in.clear();
	}
	std::cout << "Done loading turrets. \nLoading weapons... \n";
	for (const auto& file : std::filesystem::directory_iterator(weaponpath)) {
		loadWeaponData(file.path().string(), in);
		in.clear();
	}
	std::cout << "Done loading weapons. \nLoading obstacles... \n";
	for (const auto& file : std::filesystem::directory_iterator(obstpath)) {
		u32 id = loadObstacleData(file.path().string(), in);
		in.clear();
		if (id == -1) continue;
		ObstacleData* data = obstacleData[id];
		if (data->type != GAS_CLOUD) {
			btConvexHullShape hull;
			std::string fname = hullpath + data->name + ".bullet";
			if (loadHull(fname, hull)) {
				std::cout << "Loaded hull for " << file.path().string() << ".\n";
				data->shape = hull;
			} else {
				std::cout << "Could not load hull for " << file.path().string() << ". Building hull... ";
				IMesh* mesh = smgr->getMesh(data->obstacleMesh.c_str());
				data->shape = createCollisionShapeFromMesh(mesh);
				saveHull(fname, data->shape);
				smgr->getMeshCache()->removeMesh(mesh);
				std::cout << "Done. New hull saved to " << fname << ".\n";
			}
		}
	}
	std::cout << "Number of weapons: " << weaponData.size() << std::endl;
	std::cout << "Number of physics weapons: " << physWeaponData.size() << std::endl;
	std::cout << "Number of ships: " << shipData.size() << std::endl;
	std::cout << "Number of obstacle types: " << obstacleData.size() << std::endl;
	std::cout << "Number of turrets: " << turretData.size() << std::endl;
	std::cout << "Number of carriers: " << carrierData.size() << std::endl;
}

bool GameStateController::OnEvent(const SEvent& event)
{
	if (!gameInitialized) return true;

	if (event.EventType == EET_KEY_INPUT_EVENT) {
		if (event.KeyInput.Key == KEY_ESCAPE && event.KeyInput.PressedDown) {
			if (state == GAME_RUNNING) {
				setState(GAME_PAUSED); //Overrides anything else and sets the game to paused when the escape key is hit - needs abstraction later
			}
			else if (state == GAME_PAUSED) {
				setState(GAME_RUNNING);
			}
		}
	}
	switch (state) { //Passes events to their respective controllers and tells them its THEIR problem now
	case GAME_MENUS:
		guiController->OnEvent(event);
		break;
	case GAME_RUNNING:
		gameController->OnEvent(event);
		break;
	case GAME_PAUSED:
		guiController->OnEvent(event);
		break;
	case GAME_FINISHED:
		guiController->OnEvent(event);
		break;
	}
	return false;
}

void GameStateController::setState(GAME_STATE newState)
{
	oldState = state;
	state = newState;
	stateChangeCalled = true; //Lets the stateController know it needs to update the state. Can be called anywhere
}

void GameStateController::backToCampaign()
{
	audioDriver->cleanupGameSounds();
	campaign->getSector()->finishScenario();
	returningToCampaign = true;
	setState(GAME_MENUS);
}

void GameStateController::stateChange() //Messy handler for the different states; since there aren't many it's just an if chain
{
	if (oldState == GAME_MENUS && state == GAME_RUNNING) {
		guiController->close();
		gameController->init();
		gameController->initScenario();
	}
	else if (oldState == GAME_PAUSED && state == GAME_MENUS) {
		gameController->close();
		guiController->setActiveDialog(GUI_MAIN_MENU);
	}
	else if (oldState == GAME_RUNNING && state == GAME_PAUSED) {
		guiController->setActiveDialog(GUI_PAUSE_MENU);
	}
	else if (oldState == GAME_PAUSED && state == GAME_RUNNING) {
		guiController->close();
	}
	else if (oldState == GAME_RUNNING && state == GAME_FINISHED) { 
		guiController->setActiveDialog(GUI_DEATH_MENU);
		//set up death menu
	}
	else if (oldState == GAME_FINISHED && state == GAME_MENUS) {
		if (returningToCampaign) {
			guiController->setActiveDialog(GUI_LOOT_MENU);
			returningToCampaign = false;
		} else {
			guiController->setActiveDialog(GUI_MAIN_MENU);
		}
		gameController->close();
	}
	stateChangeCalled = false;
}

void GameStateController::mainLoop()
{
	u32 lastFPS = -1;
	while (device->run()) {
		//audioDriver->audioUpdate();
		if (stateChangeCalled) {
			stateChange(); //Updates state if the change has been called by one of the controllers
		}

		u32 now = device->getTimer()->getTime();
		switch (state) { //Calls updates from its controllers based on the current state
			case GAME_MENUS:
				guiController->update();
				break;
			case GAME_RUNNING:
				gameController->update();
#ifdef _DEBUG
				bWorld->debugDrawWorld();
#endif
				break;
			case GAME_PAUSED:
				guiController->update();
				break;
			case GAME_FINISHED:
				guiController->update();
				break;
		}
		then = now;
#if _DEBUG
		driver->setTransform(ETS_WORLD, IdentityMatrix);
		SMaterial material;
		material.Lighting = false;
		material.BackfaceCulling = false;
		material.ZBuffer = ECFN_ALWAYS;
		driver->setMaterial(material);
#endif 
		driver->beginScene(true, true, SColor(255, 20, 20, 20));
		smgr->drawAll();
		guienv->drawAll();
#if _DEBUG
		for (line3df line : debugLines) {
			driver->draw3DLine(line.start, line.end);
		}
		debugLines.clear();
#endif 
		/*
		if (now - then > 100) {
			driver->runAllOcclusionQueries(false);
			driver->updateAllOcclusionQueries();
		}
		*/
		driver->endScene();
		int fps = driver->getFPS();
		stringw tmp(L"Flight [");
		tmp += driver->getName();
		tmp += L"] FPS: ";
		if (lastFPS != fps) {
			tmp += fps;
		}
		else tmp += lastFPS;

		device->setWindowCaption(tmp.c_str());
		lastFPS = fps;
	}
}