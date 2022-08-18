#pragma once

#ifndef BASEFLIGHTHEADER_H
#define BASEFLIGHTHEADER_H

/*
* This header holds all of the basic includes that are needed for the project.
* When adding new files, this shoudl be the first thing you include.
*/
#include <irrlicht.h>
using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btShapeHull.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <Serialize/BulletWorldImporter/btBulletWorldImporter.h>
#include <irrKlang.h>
#include <flecs.h>

#include <al.h>
#include <alc.h>

#include <efx.h>
#include <EFX-Util.h>
#include <efx-creative.h>

#include <ogg.h>
#include <vorbisfile.h>
#include <vorbisenc.h>

using namespace irrklang;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")

#if _DEBUG
#pragma comment(lib, "BulletDynamics_Debug.lib")
#pragma comment(lib, "BulletCollision_Debug.lib")
#pragma comment(lib, "LinearMath_Debug.lib")
#pragma comment(lib, "BulletWorldImporter_Debug.lib")
#pragma comment(lib, "BulletFileLoader_Debug.lib")

#pragma comment(lib, "irrKlang.lib")
#else 
#pragma comment(lib, "BulletDynamics.lib")
#pragma comment(lib, "BulletCollision.lib")
#pragma comment(lib, "LinearMath.lib")
#pragma comment(lib, "BulletWorldImporter.lib")
#pragma comment(lib, "BulletFileLoader.lib")
#endif 

#pragma comment(lib, "irrKlang.lib")

#pragma comment(lib, "OpenAL32.lib")
#pragma comment(lib, "EFX-Util.lib")
#pragma comment(lib, "libogg_static.lib")
#pragma comment(lib, "libvorbis_static.lib")
#pragma comment(lib, "libvorbisfile_static.lib")
#endif

#include <unordered_map>

class GameStateController;
class GameController;
class GuiController;
class SceneManager;
class BulletPhysicsWorld;
class Assets;
class Campaign;
class AudioDriver;
/*
* These are global variables used from throughout the program to track what exactly is going on.
*/

extern GameStateController* stateController;
extern GameController* gameController;
extern GuiController* guiController;

extern Assets* assets;
extern Campaign* campaign;

extern IrrlichtDevice* device;
extern IVideoDriver* driver;
extern ISceneManager* smgr;
extern IGUIEnvironment* guienv;
extern ISoundEngine* soundEngine;
extern BulletPhysicsWorld* bWorld;
extern flecs::world* game_world;

extern AudioDriver* audioDriver;

struct ShipData;
struct CarrierData;
struct TurretData;
struct WeaponData;
struct ObstacleData;

extern std::unordered_map<u32, ShipData*> shipData;
extern std::unordered_map<u32, CarrierData*> carrierData;
extern std::unordered_map<u32, TurretData*> turretData;
extern std::unordered_map<u32, WeaponData*> weaponData;
extern std::unordered_map<u32, WeaponData*> physWeaponData;
extern std::unordered_map<u32, ObstacleData*> obstacleData;

const flecs::id_t INVALID_ENTITY_ID = 0;
#define INVALID_ENTITY flecs::entity(INVALID_ENTITY_ID)

#endif