#include "GuiLootMenu.h"
#include "GameStateController.h"
#include "GameController.h"
#include "GuiController.h"

void GuiLootMenu::init()
{
	screen = guienv->addImage(rect<s32>(position2di((960 - 400) / 2, -500), dimension2du(400, 500)), root);
	screen->setImage(driver->getTexture("ui/vertscreen.png"));
	scaleAlign(screen);

	dimension2du buttonSize(300, 80);
	loot = guienv->addStaticText(L"Loot", rect<s32>(position2di(50, 5), dimension2du(300, 350)), false, true, screen);
	setUIText(loot);
	returnToCampaign = guienv->addButton(rect<s32>(position2di(50, 354), buttonSize), screen, -1, L"Back to Campaign", L"Return to the campaign.");
	setHoloButton(returnToCampaign);
	guiController->setCallback(returnToCampaign, std::bind(&GuiLootMenu::onReturnToCampaign, this, std::placeholders::_1));

	guiController->setCloseAnimationCallback(this, std::bind(&GuiLootMenu::onHide, this, std::placeholders::_1));
	guiController->setOpenAnimationCallback(this, std::bind(&GuiLootMenu::onShow, this, std::placeholders::_1));
	hide();
}

void GuiLootMenu::show()
{
	root->setRelativePosition(rect<s32>(position2di(0, 0), driver->getScreenSize()));
	root->setVisible(true);

	Scenario scen = campaign->getSector()->getCurrentScenario();
	u32 wepsRecovered = 0; 
	if(scen.maxWepsRecovered > 0) wepsRecovered = std::rand() % scen.maxWepsRecovered;
	u32 shipsRecovered = 0; 
	if (scen.maxShipsRecovered > 0) shipsRecovered = std::rand() % scen.maxShipsRecovered;
	for (u32 i = 0; i < wepsRecovered; ++i) {
		auto wepinst = campaign->createRandomWeaponInstance();
		weps.push_back(wepinst->wep);
		campaign->addWeapon(wepinst);
	}
	for (u32 i = 0; i < shipsRecovered; ++i) {
		auto shipinst = campaign->createRandomShipInstance();
		ships.push_back(shipinst);
		campaign->addShipInstanceToHangar(shipinst);
	}
	std::string txt = "Objective Completed \n \n";
	txt += "\n Ammo recovered: " + std::to_string(scen.ammoRecovered);
	campaign->addAmmo(scen.ammoRecovered);
	txt += "\n Resources recovered: " + fprecis(scen.resourcesRecovered, 5);
	campaign->addSupplies(scen.resourcesRecovered);

	if (wepsRecovered > 0) {
		txt += "\n \n Weapons recovered:";
		for (WeaponInfoComponent wep : weps) {
			txt += "\n" + weaponData[wep.wepDataId]->name;
			if (wep.usesAmmunition) {
				txt += " Ammo: " + std::to_string(wep.ammunition);
			}
		}
	}
	if (shipsRecovered > 0) {
		txt += "\n \n Ships recovered:";
		for (ShipInstance* inst : ships) {
			txt += "\n" + shipData[inst->ship.shipDataId]->name;
			txt += " Health: " + fprecis(inst->hp.health, 5);
		}
	}
	loot->setText(wstr(txt).c_str());
}

bool GuiLootMenu::onReturnToCampaign(const SEvent& event)
{
	if (event.GUIEvent.EventType != EGET_BUTTON_CLICKED) return true;
	campaign->returnToCampaign();
	guiController->setActiveDialog(GUI_CAMPAIGN_MENU);
	return false;
}

bool GuiLootMenu::onShow(f32 dt)
{
	rect<s32> pos = root->getAbsolutePosition();
	rect<s32> imgpos = screen->getRelativePosition();
	position2di curpos = imgpos.UpperLeftCorner;
	f32 vertprop = 500.f / 540.f;
	s32 dist = (s32)(pos.getHeight() * vertprop);
	return smoothGuiMove(screen, .25f, timer, position2di(curpos.X, -dist), position2di(curpos.X, -5), dt);
}

bool GuiLootMenu::onHide(f32 dt)
{
	rect<s32> pos = root->getAbsolutePosition();
	rect<s32> imgpos = screen->getRelativePosition();
	position2di curpos = imgpos.UpperLeftCorner;
	f32 vertprop = 500.f / 540.f;
	s32 dist = (s32)(pos.getHeight() * vertprop);
	return smoothGuiMove(screen, .25f, timer, position2di(curpos.X, -5), position2di(curpos.X, -dist), dt);
}