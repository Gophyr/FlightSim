#pragma once
#ifndef CARRIERUTILS_H
#define CARRIERUTILS_H
#include "BaseHeader.h"
#include "CarrierComponent.h"

//Builds a carrier from the ID of the carrier.
flecs::entity createCarrierFromId(u32 id, vector3df pos, vector3df rot);

flecs::entity createTurretFromId(u32 id, flecs::entity owner, vector3df pos, vector3df startingRot);

//Sets up a default human carrier.
flecs::entity createHumanCarrier(u32 carrId, vector3df pos, vector3df rot);
//Sets up a default alien character.
flecs::entity createAlienCarrier(u32 carrId, vector3df pos, vector3df rot);

//The death callback for carriers; causes a large explosion and a different noise.
void carrierDeathExplosionCallback(flecs::entity id);
#endif