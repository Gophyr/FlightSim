#include "BaseHeader.h"
#include "GameStateController.h"
#include "GvReader.h"
#include "Config.h"
#include "AudioDriver.h"

GameStateController* stateController = 0;
GameController* gameController = 0;
GuiController* guiController = 0;

IrrlichtDevice* device = 0;
IVideoDriver* driver = 0;
ISceneManager* smgr = 0;
IGUIEnvironment* guienv = 0;
ISoundEngine* soundEngine = 0;
BulletPhysicsWorld* bWorld = 0;
flecs::world* game_world = 0;

std::unordered_map<u32, ShipData*> shipData;
std::unordered_map<u32, CarrierData*> carrierData;
std::unordered_map<u32, TurretData*> turretData;
std::unordered_map<u32, WeaponData*> weaponData;
std::unordered_map<u32, WeaponData*> physWeaponData;
std::unordered_map<u32, ObstacleData*> obstacleData;

Assets* assets = new Assets;

Campaign* campaign = new Campaign;

AudioDriver* audioDriver = new AudioDriver();

int main()
{
	VideoConfig config;
	config.loadConfig("cfg/videoconfig.gdat");
	config.saveConfig("cfg/videoconfig.gdat");
	dimension2du res(config.resX, config.resY);
	if (config.useScreenRes) {
		IrrlichtDevice* nullDev = createDevice(EDT_NULL); //Used to get the current screen res if needed
		res = nullDev->getVideoModeList()->getDesktopResolution();
		nullDev->drop();
	}
	device = createDevice(config.driver, res, 32, config.fullscreen, config.stencilBuffer, config.vsync, 0);
	driver = device->getVideoDriver();
	smgr = device->getSceneManager();
	guienv = device->getGUIEnvironment();

	audioDriver->init();

	stateController = new GameStateController(config);
	stateController->videoConfig = config;
	stateController->init();
	stateController->mainLoop();
	device->drop();
	return 0;

}