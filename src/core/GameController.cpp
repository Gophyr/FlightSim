#include "GameController.h"
#include "GameStateController.h"
#include "ObstacleUtils.h"

#include <iostream>
#include <random>

GameController::GameController()
{
	open = false;
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
		bWorld->stepSimulation(dt, 60);
		game_world->progress(dt);
		t += dt;
		accumulator -= dt;
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

	currentScenario = stateController->campaign.currentScenario;

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

	collCb = new broadCallback();
	bWorld->getPairCache()->setOverlapFilterCallback(collCb);
	registerComponents();
	registerSystems();
	open = true;
}

void GameController::registerComponents()
{
	game_world->component<AIComponent>();
	game_world->component<BulletGhostComponent>();
	game_world->component<BulletRigidBodyComponent>();
	game_world->component<DamageTrackingComponent>();
	game_world->component<ExplosionComponent>();
	game_world->component<HealthComponent>();
	game_world->component<IrrlichtComponent>();
	game_world->component<ObjectiveComponent>();
	game_world->component<ObstacleComponent>();
	game_world->component<InputComponent>();
	game_world->component<PlayerComponent>();
	game_world->component<CarrierComponent>();
	game_world->component<FactionComponent>();
	game_world->component<SensorComponent>();
	game_world->component<ShipComponent>();
	game_world->component<ShipParticleComponent>();
	game_world->component<BolasInfoComponent>();
	game_world->component<KineticInfoComponent>();
	game_world->component<MissileInfoComponent>();
	game_world->component<MissileProjectileComponent>();
	game_world->component<WeaponInfoComponent>();
	game_world->component<ProjectileInfoComponent>();
	//not added: turret, harpoon
}
void GameController::registerSystems()
{
	/*
*	flecs::OnLoad
*	flecs::PostLoad
*	flecs::PreUpdate
*	flecs::OnUpdate
*	flecs::OnValidate
*	flecs::PostUpdate
*	flecs::PreStore
*	flecs::OnStore
*/
	auto playerctrl = game_world->system<InputComponent, ShipComponent, PlayerComponent, BulletRigidBodyComponent, IrrlichtComponent, SensorComponent>()
		.no_staging(false).kind(flecs::PreUpdate).iter(shipControlSystem);
	auto aictrl = game_world->system<AIComponent, IrrlichtComponent, BulletRigidBodyComponent, ShipComponent, SensorComponent, HealthComponent>()
		.no_staging(false).kind(flecs::PreUpdate).iter(AIUpdateSystem);

	game_world->system<WeaponInfoComponent, IrrlichtComponent>().kind(flecs::OnUpdate).iter(weaponFiringSystem);
	game_world->system<CarrierComponent, IrrlichtComponent, FactionComponent>().kind(flecs::OnUpdate).iter(carrierUpdateSystem);
	game_world->system<BulletRigidBodyComponent, SensorComponent, FactionComponent>().kind(flecs::OnUpdate).iter(sensorSystem);
	game_world->system().kind(flecs::OnUpdate).iter(collisionCheckingSystem);
	game_world->system<DamageTrackingComponent, HealthComponent>().kind(flecs::OnUpdate).iter(damageSystem);
	game_world->system<ExplosionComponent>().kind(flecs::OnUpdate).iter(explosionSystem);
	game_world->system<BulletRigidBodyComponent, IrrlichtComponent>().kind(flecs::OnUpdate).iter(irrlichtRigidBodyPositionSystem);
	game_world->system<IrrlichtComponent, PlayerComponent, BulletRigidBodyComponent, SensorComponent>().kind(flecs::OnUpdate).iter(playerUpdateSystem);
	game_world->system<BulletRigidBodyComponent, ProjectileInfoComponent, IrrlichtComponent>().kind(flecs::OnUpdate).iter(projectileSystem);

	game_world->system<ShipComponent, BulletRigidBodyComponent, IrrlichtComponent, ShipParticleComponent>().kind(flecs::OnUpdate).iter(shipUpdateSystem);
	game_world->system().kind(flecs::OnUpdate).iter(soundSystem);
	game_world->system<HealthComponent>().kind(flecs::OnUpdate).iter(healthSystem);
	game_world->system<ShieldComponent>().kind(flecs::OnUpdate).iter(shieldSystem);
	game_world->system<ObjectiveComponent>().kind(flecs::OnUpdate).iter(objectiveSystem);
}

void GameController::close()
{
	if (!open) return;

	clearPlayerHUD();
	smgr->clear();
	bWorld->clearObjects();
	delete broadPhase;
	delete collisionConfig;
	delete dispatcher;
	delete solver;
	delete bWorld; //this likely leaks some memory
	delete collCb;
	delete gPairCb;

	//todo: need to clean out the ECS
	sounds.clear();
	stateController->assets.clearLoadedGameAssets();
	open = false;
}

void GameController::setPlayer(flecs::entity_t id)
{
	playerEntity = id;
}
flecs::entity GameController::getPlayer()
{
	return flecs::entity(game_world->get_world(), playerEntity);
}

void GameController::clearPlayerHUD()
{
	if (!getPlayer().is_alive()) return;

	auto plyc = getPlayer().get_mut<PlayerComponent>();
	for (HUDElement* hud : plyc->HUD) {
		delete hud;
	}
	plyc->rootHUD->remove();
}

bool GameController::OnEvent(const SEvent& event)
{
	if (!open) return true;
	if (!getPlayer().is_alive()) return true;
	if(!getPlayer().has<InputComponent>()) return true;
	auto input = getPlayer().get_mut<InputComponent>();
	if (event.EventType == EET_KEY_INPUT_EVENT) {
		input->keysDown[event.KeyInput.Key] = event.KeyInput.PressedDown;
		if(event.KeyInput.Key == KEY_KEY_Y && !input->keysDown[KEY_KEY_Y]) {
			input->mouseControlEnabled = !input->mouseControlEnabled;
		}
		if (event.KeyInput.Key == KEY_KEY_U && !input->keysDown[KEY_KEY_U]) {
			input->safetyOverride = !input->safetyOverride;
		}
	}
	if (event.EventType == EET_MOUSE_INPUT_EVENT) {
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
	if (event.EventType == EET_GUI_EVENT) {
		//handle GUI events here, but there probably aren't any that the HUD itself doesn't handle
	}
	return false;
}

void GameController::initScenario()
{
	std::cout << "Loading scenario...\n";
	isPlayerAlive = true;
	std::cout << "scenario type: " << currentScenario.type << ", scenario environment: " << currentScenario.environment << ", objective count: " << currentScenario.objectiveCount;
	std::cout << "\n player start: " << currentScenario.playerStartPos.X << " " << currentScenario.playerStartPos.Y << " " << currentScenario.playerStartPos.Z;
	std::cout << "\n enemy start: " << currentScenario.enemyStartPos.X << " " << currentScenario.enemyStartPos.Y << " " << currentScenario.enemyStartPos.Z;
	std::cout << "\n num obstacles: " << currentScenario.obstaclePositions.size();
	std::cout << "\n ammo to be recovered: " << currentScenario.ammoRecovered << " resources: " << currentScenario.resourcesRecovered;
	std::cout << "\n max ships recovered: " << currentScenario.maxShipsRecovered << " max weps: " << currentScenario.maxWepsRecovered << std::endl;

	buildScenario(currentScenario);

	device->getCursorControl()->setActiveIcon(ECI_CROSS);
	std::cout << "Scenario loaded. \n";
}