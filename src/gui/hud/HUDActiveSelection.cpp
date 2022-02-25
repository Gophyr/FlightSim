#include "HUDActiveSelection.h"
#include "SceneManager.h"
#include "GameController.h"
#include <iostream>

HUDActiveSelection::HUDActiveSelection(SceneManager* man, IGUIElement* root) : HUDElement(man, root)
{
	selectGUI = man->controller->guienv->addImage(man->defaults.defaultSelectionTexture, position2di(0, 0), root);
	name = man->controller->guienv->addStaticText(L"", rect<s32>(position2di(0, 0), dimension2du(128, 128)), false, false, root);
	name->setOverrideColor(SColor(255, 255, 255, 255));
	name->enableOverrideColor(true);
	activeSelection = INVALID_ENTITY;
	selectGUI->setVisible(false);
	name->setVisible(false);
}

HUDActiveSelection::~HUDActiveSelection()
{
	selectGUI->remove();
	name->remove();
}

void HUDActiveSelection::updateElement(SceneManager* manager, EntityId playerId)
{
	auto player = manager->scene.get<PlayerComponent>(playerId);
	auto input = manager->scene.get<InputComponent>(playerId);
	auto playerIrr = manager->scene.get<IrrlichtComponent>(playerId);

	ICameraSceneNode* camera = player->camera;

	ISceneCollisionManager* coll = manager->controller->smgr->getSceneCollisionManager();
	line3df ray = coll->getRayFromScreenCoordinates(input->mousePixPosition, camera);

	if (!manager->scene.entityInUse(activeSelection)) { //Check to see if the entity still exists
		activeSelection = INVALID_ENTITY;
	}

	if (input->rightMouseDown) { //Uses the input component to hurl out a ray selecting anything in its path
		ISceneNode* selection = coll->getSceneNodeFromRayBB(ray, ID_IsSelectable);
		if (selection) {
			if (selection->getID() != -1 && selection != playerIrr->node) {
				EntityId id = strToId(selection->getName());
				if(manager->scene.entityInUse(id)) activeSelection = id;
				auto irr = manager->scene.get<IrrlichtComponent>(id);
				std::wstring widestr = std::wstring(irr->name.begin(), irr->name.end());
				name->setText(widestr.c_str());
				selectGUI->setVisible(true);
				name->setVisible(true);
			}
		}
		else if (!selection) {
			selectGUI->setVisible(false);
			name->setVisible(false);
			activeSelection = INVALID_ENTITY;
		}
	}
	if (activeSelection == INVALID_ENTITY) {
		selectGUI->setVisible(false);
		name->setVisible(false);
		return;
	}
	auto irr = manager->scene.get<IrrlichtComponent>(activeSelection);
	if (!irr) {
		activeSelection = INVALID_ENTITY;
		return;
	}
	//Moves around the selection GUI
	position2di selectionPos = coll->getScreenCoordinatesFrom3DPosition(irr->node->getAbsolutePosition(), camera);
	selectionPos.X -= 64;
	selectionPos.Y -= 64;
	selectGUI->setRelativePosition(selectionPos);
	selectionPos.Y -= 16;
	name->setRelativePosition(selectionPos);
}