#pragma once
#ifndef ATTRIBUTEREADERS_H
#define ATTRIBUTEREADERS_H
#include "BaseHeader.h"
#include "GvReader.h"
#include "ShipComponent.h"
#include "WeaponInfoComponent.h"
#include "IrrlichtComponent.h"
#include "LoadoutData.h"


class SceneManager;
class GameController;
class GameStateController;

/*
* These functions are used to load various chunks of data using the GvReader class. Can load ships, weapons, and other stuff
* from .gdat files.
*/

//Reads a vector from a string in the manner of: 40,50,-20
vector3df strToVec(std::string str);
//Loads the ship data from the game state controller onto the given entity.
//Includes an Irrlicht component andg6 a ShipComponent.
bool loadShip(u32 id, EntityId entity);
//Loads the weapon data from the game state controller onto the given entity.
//Includes a WeaponInfoComponent, an Irrlicht component, a ParentComponent, and whatever other components are necessary (e.g., a MissileInfoComponent).
bool loadWeapon(u32 id, EntityId weaponEntity, EntityId shipEntity, bool phys);

//Loads the obstacle data from the game state controller onto the given entity.
bool loadObstacle(u32 id, EntityId entity);

//Pulls the ship data from the given .gdat file and saves it in the game state controller. Returns the ID.
u32 loadShipData(std::string path, gvReader& in);
//Pulls the weapon data from the given .gdat file and saves it in the game state controller. Returns the ID.
u32 loadWeaponData(std::string path, gvReader& in);
//Pulls out obstacle data from a given .gdat file and saves it in the game state controller. Returns the ID.
u32 loadObstacleData(std::string path, gvReader& in);

//Pulls out loadout data from the given .gdat file and saves it in the game state controller. Returns the ID.
LoadoutData loadLoadoutData(std::string path);

//Creates a convex hull shape from an Irrlicht mesh and simplifies it down to something usable.
//This should only be used if there isn't a hitbox mesh available.
btConvexHullShape createCollisionShapeFromMesh(IMesh* mesh);

//Saves a convex hull to file. Returns true if successful.
bool saveHull(std::string path, btConvexHullShape& shape);
//Loads a convex hull from file onto the "shape" hull. Returns true if successful.
bool loadHull(std::string path, btConvexHullShape& shape);


#endif 