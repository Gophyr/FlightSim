#pragma once

#ifndef HUDELEMENT_H
#define HUDELEMENT_H
#include "BaseHeader.h"
#include "PlayerComponent.h"
#include "InputComponent.h"
#include "ECS.h"

class SceneManager;

enum class HUD_ELEM_TYPE
{
	ACTIVE_SELECTION,
	CONTACT,
	HEALTH_BAR,
	VELOCITY_BAR
};

/*
* The base class for all HUD elements. HUD elements should inherit from this class.
* Includes the root node for the HUD element as part of the constructor, but doesn't actually HOLD
* the root HUD element. The root HUD element is part of the player class, and should be used to show or hide
* the entire HUD at once.
*/
class HUDElement
{
public:
	HUD_ELEM_TYPE type;
	HUDElement(SceneManager* man, IGUIElement* root) {}
	virtual ~HUDElement() {}
	virtual void updateElement(SceneManager* manager, EntityId playerId) = 0;
};

#endif