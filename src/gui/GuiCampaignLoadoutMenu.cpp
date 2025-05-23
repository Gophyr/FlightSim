#include "GuiCampaignLoadoutMenu.h"
#include "GameStateController.h"
#include "GameController.h"
#include "GuiController.h"

void GuiCampaignLoadoutMenu::init()
{
	dimension2du buttonSize(160, 40);
	dimension2du smallButtonSize(60, 40);
	back = guienv->addButton(rect<s32>(position2di(0, 0), buttonSize), root, -1, L"Back", L"Return to campaign menu.");
	wepMenu = guienv->addImage(rect<s32>(position2di(10, 120), dimension2du(237, 319)), root);
	physWepMenu = guienv->addImage(rect<s32>(position2di(713, 120), dimension2du(237, 319)), root);
	wepMenu->setImage(driver->getTexture("ui/freestandingmenu.png"));
	physWepMenu->setImage(driver->getTexture("ui/freestandingmenu.png"));
	scaleAlign(wepMenu);
	scaleAlign(physWepMenu);
	setHybridButton(back, true);

	position2di pos(280, 140);
	position2di newpos;
	s32 buf = 8;
	for (u32 i = 0; i < MAX_HARDPOINTS; ++i) {
		newpos = pos;
		newpos.Y += (buf * i) + (buttonSize.Height * i);
		hardpoints[i] = guienv->addButton(rect<s32>(newpos, buttonSize), root, -1, L"Hardpoint", L"Hardpoint attachment for a weapon.");
		setHybridButton(hardpoints[i]);
		hardpoints[i]->setID((s32)i);
		guiController->setCallback(hardpoints[i], std::bind(&GuiCampaignLoadoutMenu::onHardpointSelect, this, std::placeholders::_1));
		newpos.X += buttonSize.Width;
		newpos.Y += 10;
		reloadHardpoints[i] = guienv->addButton(rect<s32>(newpos, dimension2du(20, 20)), root, (s32)i, L"", L"Reload this weapon.");
		setReloadButton(reloadHardpoints[i]);
		guiController->setCallback(reloadHardpoints[i], std::bind(&GuiCampaignLoadoutMenu::onReload, this, std::placeholders::_1));
		newpos.X -= buttonSize.Width;
		newpos.Y -= 10;
	}
	position2di rpos(520, 140);
	physWepHardpoint = guienv->addButton(rect<s32>(rpos, buttonSize), root, -1, L"Phys Wep", L"Hardpoint for the physics weapon.");
	physWepHardpoint->setID(PHYS_HARDPOINT);
	setHybridButton(physWepHardpoint);
	guiController->setCallback(physWepHardpoint, std::bind(&GuiCampaignLoadoutMenu::onHardpointSelect, this, std::placeholders::_1));
	pos.Y -= 60;
	rpos.Y -= 60;
	rpos.X += 100;
	shipSelector.left = guienv->addButton(rect<s32>(pos, smallButtonSize), root, -1, L"<");
	shipSelector.right = guienv->addButton(rect<s32>(rpos, smallButtonSize), root, -1, L">");
	rpos.Y += 10;
	rpos.X += smallButtonSize.Width;
	repair = guienv->addButton(rect<s32>(rpos, dimension2du(20, 20)), root, -1, L"", L"Repair this ship.");
	setRepairButton(repair);
	guiController->setCallback(repair, std::bind(&GuiCampaignLoadoutMenu::onRepair, this, std::placeholders::_1));

	pos.X += smallButtonSize.Width;
	shipSelector.name = guienv->addStaticText(L"Ship Instance", rect<s32>(pos, dimension2du(280, 40)), false, true, root);

	setUIText(shipSelector.name);
	setMetalButton(shipSelector.left);
	setMetalButton(shipSelector.right);

	newpos.Y += buttonSize.Height + buf;
	pos.Y = 0;
	pos.X -= smallButtonSize.Width;
	shipInfo = guienv->addStaticText(L"Ship Information", rect<s32>(pos, dimension2du(400, 70)), true, true, root);
	wepDesc = guienv->addStaticText(L"", rect<s32>(newpos, dimension2du(400, 70)), false, true, root);
	setUIText(wepDesc);
	setUIText(shipInfo);
	pos.X -= 100;
	pos.Y += 40;
	shipSelector.useShip = guienv->addButton(rect<s32>(pos, dimension2du(100, 25)), root, -1, L"Use this ship", L"Use this ship as your own.");
	setMetalButton(shipSelector.useShip);
	guiController->setCallback(shipSelector.useShip, std::bind(&GuiCampaignLoadoutMenu::onUseShip, this, std::placeholders::_1));

	wepMenuBg = guienv->addStaticText(L"", rect<s32>(position2di(20, 20), dimension2du(187, 267)), false, true, wepMenu);
	setUIText(wepMenuBg); //not really necessary; the bg just provides something to tape wep loadout stuff into
	physWepMenuBg = guienv->addStaticText(L"", rect<s32>(position2di(20, 20), dimension2du(187, 267)), false, true, physWepMenu);
	setUIText(physWepMenuBg);

	carrierInfoPanel = guienv->addImage(rect<s32>(position2di(758, 0), dimension2du(202, 88)), root);
	scaleAlign(carrierInfoPanel);
	carrierInfoPanel->setImage(driver->getTexture("ui/infopanel.png"));
	carrierInfo = guienv->addStaticText(L"", rect<s32>(position2di(35, 0), dimension2du(165, 65)), false, true, carrierInfoPanel);
	setUIText(carrierInfo);

	guiController->setCallback(back, std::bind(&GuiCampaignLoadoutMenu::onBack, this, std::placeholders::_1));
	guiController->setCallback(shipSelector.left, std::bind(&GuiCampaignLoadoutMenu::onShipChangeLeft, this, std::placeholders::_1));
	guiController->setCallback(shipSelector.right, std::bind(&GuiCampaignLoadoutMenu::onShipChangeRight, this, std::placeholders::_1));
	hide();
}

bool GuiCampaignLoadoutMenu::onBack(const SEvent& event)
{
	if (event.GUIEvent.EventType != EGET_BUTTON_CLICKED) return true;
	guiController->setActiveDialog(GUI_CAMPAIGN_MENU);
	return false;
}

void GuiCampaignLoadoutMenu::show()
{
	root->setRelativePosition(rect<s32>(position2di(0, 0), driver->getScreenSize()));
	root->setVisible(true);

	//initialize first loadout visible as player ship'
	shipSelector.curPos = campaign->ships().begin();
	displayShip(campaign->getPlayerShip());
	displayCarrierInfo();
}

void GuiCampaignLoadoutMenu::displayShip(ShipInstance* inst)
{
	if (!inst) {
		std::cout << "No ship instance!!\n";
	}
	u32 shipId = inst->ship.shipDataId;
	ShipData* data = shipData[shipId];
	std::string name = data->name;
	if (inst == campaign->getPlayerShip()) {
		shipSelector.useShip->setVisible(false);
		name += " - Player";
	}
	else {
		shipSelector.useShip->setVisible(true);
	}
	shipSelector.name->setText(wstr(name).c_str());

	for (u32 i = 0; i < inst->hards.hardpointCount; ++i) {
		WeaponInfoComponent wep = inst->weps[i]->wep;
		std::string name = weaponData[wep.wepDataId]->name;
		hardpoints[i]->setText(wstr(name).c_str());
		hardpoints[i]->setVisible(true);
		if (wep.usesAmmunition) {
			reloadHardpoints[i]->setVisible(true);
		}
		else {
			reloadHardpoints[i]->setVisible(false);
		}

	}
	for (u32 i = inst->hards.hardpointCount; i < MAX_HARDPOINTS; ++i) {
		hardpoints[i]->setVisible(false);
		reloadHardpoints[i]->setVisible(false);
	}
	std::string phys = physWeaponData[inst->physWep->wep.wepDataId]->name;
	physWepHardpoint->setText(wstr(phys).c_str());

	std::string desc = name + "\n" + data->description + "\n HP: " + fprecis(inst->hp.health, 5) + "/" + fprecis(inst->hp.maxHealth, 5);
	shipInfo->setText(wstr(desc).c_str());
}
void GuiCampaignLoadoutMenu::displayCarrierInfo()
{
	std::string txt = "Carrier Info \n Total supplies: " 
		+ fprecis(campaign->getSupplies(), 5) 
		+ "\n Total ammo available: " 
		+ std::to_string(campaign->getAmmo());
	carrierInfo->setText(wstr(txt).c_str());
}

bool GuiCampaignLoadoutMenu::onShipChange(const SEvent& event, bool right)
{
	if (event.GUIEvent.EventType != EGET_BUTTON_CLICKED || campaign->ships().size() == 0) return true;

	if (right) {
		++shipSelector.curPos;
		if (shipSelector.curPos == campaign->ships().end()) shipSelector.curPos = campaign->ships().begin();
	}
	else {
		if (shipSelector.curPos == campaign->ships().begin()) {
			shipSelector.curPos = --campaign->ships().end();
		}
		else {
			--shipSelector.curPos;
		}
	}

	displayShip(getCurShip());
	return false;
}
bool GuiCampaignLoadoutMenu::onShipChangeLeft(const SEvent& event)
{
	return onShipChange(event, false);
}
bool GuiCampaignLoadoutMenu::onShipChangeRight(const SEvent& event)
{
	return onShipChange(event, true);
}

WepSelect GuiCampaignLoadoutMenu::buildWepSelect(WeaponInfoComponent& wep, position2di pos, bool phys)
{
	f32 screenRatioHoriz = 187.f / 960.f;
	f32 screenRatioVert = 40.f / 540.f;
	s32 width = (s32)(screenRatioHoriz * root->getAbsolutePosition().getWidth());
	s32 height = (s32)(screenRatioVert * root->getAbsolutePosition().getHeight());

	rect<s32> bgRect(pos, dimension2du(width, height));
	WepSelect sel;
	WeaponData* data = weaponData[wep.wepDataId];
	position2di npos(0, 0);
	position2di bpos(bgRect.getWidth() / 2, 0);
	IGUIStaticText* bg = wepMenuBg;

	if (phys) {
		bg = physWepMenuBg;
		npos = bpos;
		bpos = position2di(0, 0);
		data = physWeaponData[wep.wepDataId];
	}
	sel.bg = guienv->addStaticText(L"", bgRect, false, true, bg);
	std::string txt = data->name;
	if (data->weaponComponent.usesAmmunition) {
		txt += "\n Ammo: " + std::to_string(wep.ammunition + wep.maxClip) + "/" + std::to_string(data->weaponComponent.maxAmmunition + data->weaponComponent.maxClip);
	}
	sel.name = guienv->addStaticText(wstr(txt).c_str(), rect<s32>(npos, dimension2du(bgRect.getWidth() / 2, bgRect.getHeight())), false, true, sel.bg);
	sel.select = guienv->addButton(rect<s32>(bpos, dimension2du(bgRect.getWidth() / 2, bgRect.getHeight())), sel.bg, -1, L"Select", L"Select this weapon.");
	setUIText(sel.bg);
	setUIText(sel.name);
	setHoloButton(sel.select);

	return sel;
}

ShipInstance* GuiCampaignLoadoutMenu::getCurShip()
{
	return *shipSelector.curPos; //needs to use a list iterator
}

void GuiCampaignLoadoutMenu::displayWeaponList()
{
	rect<s32> screen = root->getAbsolutePosition();
	f32 screenRatioVert = 40.f / 540.f;
	u32 num = 0;
	for (auto it : campaign->weapons()) {
		WeaponInfoComponent cmp = it->wep;
		WepSelect selector = buildWepSelect(cmp, position2di(0, num * screenRatioVert * screen.getHeight()), false);
		selector.select->setID(it->id);
		guiController->setCallback(selector.select, std::bind(&GuiCampaignLoadoutMenu::onWepSelect, this, std::placeholders::_1));
		weaponList.push_back(selector);
		++num;
	}
}

void GuiCampaignLoadoutMenu::clearWeaponList()
{
	for (WepSelect sel : physWeaponList) {
		sel.name->remove();
		guiController->removeCallback(sel.select);
		sel.select->remove();
		sel.bg->remove();
	}
	physWeaponList.clear();
	for (WepSelect sel : weaponList) {
		sel.name->remove();
		guiController->removeCallback(sel.select);
		sel.select->remove();
		sel.bg->remove();
	}
	weaponList.clear();
}

void GuiCampaignLoadoutMenu::displayPhysWeaponList()
{
	rect<s32> screen = root->getAbsolutePosition();
	f32 screenRatioVert = 40.f / 540.f;
	u32 num = 0;
	for (auto it : campaign->physWeapons()) {
		WeaponInfoComponent cmp = it->wep;
		WepSelect selector = buildWepSelect(cmp, position2di(0, num * screenRatioVert * screen.getHeight()), true);
		selector.select->setID(it->id);
		guiController->setCallback(selector.select, std::bind(&GuiCampaignLoadoutMenu::onPhysWepSelect, this, std::placeholders::_1));
		weaponList.push_back(selector);
		++num;
	}
}

bool GuiCampaignLoadoutMenu::wepHover(const SEvent& event)
{
	u32 hardpoint = (u32)event.GUIEvent.Caller->getID();
	WeaponInfoComponent wep;
	if (event.GUIEvent.EventType == EGET_ELEMENT_HOVERED) {
		ShipInstance* inst = getCurShip();
		wep = inst->weps[hardpoint]->wep;
		if (event.GUIEvent.Caller->getID() == PHYS_HARDPOINT) {
			wep = inst->physWep->wep;
		}
		u32 wepId = wep.wepDataId;
		WeaponData* data = weaponData[wepId];
		if (event.GUIEvent.Caller->getID() == PHYS_HARDPOINT) {
			data = physWeaponData[wepId];
		}
		std::string txt = data->name + "\n" + data->description;
		if (data->weaponComponent.usesAmmunition) {
			txt += "\n Ammo: " + std::to_string(wep.ammunition + wep.maxClip) + "/" + std::to_string(wep.maxAmmunition + wep.maxClip);
		}
		wepDesc->setText(wstr(txt).c_str());
		wepDesc->setVisible(true);
	}
	else if (event.GUIEvent.EventType == EGET_ELEMENT_LEFT) {
		wepDesc->setText(L"");
		wepDesc->setVisible(false);
	}
	return false;
}

bool GuiCampaignLoadoutMenu::onHardpointSelect(const SEvent& event)
{
	if (event.GUIEvent.EventType == EGET_ELEMENT_HOVERED || event.GUIEvent.EventType == EGET_ELEMENT_LEFT) return wepHover(event);

	if (event.GUIEvent.EventType != EGET_BUTTON_CLICKED) return true;

	IGUIButton* button = (IGUIButton*)event.GUIEvent.Caller;
	if (button->isPressed()) {
		button->setPressed(false);
		clearWeaponList();
		return false;
	}
	clearWeaponList();
	if (button->getID() == PHYS_HARDPOINT) {
		displayPhysWeaponList();
	} else {
		displayWeaponList();
	}
	button->setPressed(true);
	currentHardpoint = (u32)(button->getID());
	return false;
}

bool GuiCampaignLoadoutMenu::wepSelect(const SEvent& event, ShipInstance* inst)
{
	u32 weaponId = (u32)event.GUIEvent.Caller->getID();
	WeaponInstance* winst = campaign->getWeapon(weaponId);
	WeaponInfoComponent wep = winst->wep;
	if (wep.type != WEP_NONE) {
		campaign->removeWeapon(winst);
	}
	if (inst->weps[currentHardpoint]->wep.type != WEP_NONE) {
		campaign->addWeapon(inst->weps[currentHardpoint]);
	}
	inst->weps[currentHardpoint] = winst;

	std::string txt = weaponData[wep.wepDataId]->name;

	hardpoints[currentHardpoint]->setText(wstr(txt).c_str());
	if (wep.usesAmmunition) {
		reloadHardpoints[currentHardpoint]->setVisible(true);
	}
	else {
		reloadHardpoints[currentHardpoint]->setVisible(false);
	}
	clearWeaponList();
	return false;
}

bool GuiCampaignLoadoutMenu::physWepSelect(const SEvent& event, ShipInstance* inst)
{
	u32 weaponId = (u32)event.GUIEvent.Caller->getID();
	WeaponInstance* winst = campaign->getPhysWeapon(weaponId);
	WeaponInfoComponent wep = winst->wep;
	if (wep.type != WEP_NONE) {
		campaign->removeWeapon(winst);
	}
	if (inst->physWep->wep.type != WEP_NONE) {
		campaign->addWeapon(inst->physWep);
	}
	inst->physWep = winst;

	std::string txt = physWeaponData[wep.wepDataId]->name;

	physWepHardpoint->setText(wstr(txt).c_str());
	clearWeaponList();
	return false;
}

bool GuiCampaignLoadoutMenu::onWepSelect(const SEvent& event)
{
	if (event.GUIEvent.EventType != EGET_BUTTON_CLICKED) return true;

	return wepSelect(event, getCurShip());
}

bool GuiCampaignLoadoutMenu::onPhysWepSelect(const SEvent& event)
{
	if (event.GUIEvent.EventType != EGET_BUTTON_CLICKED) return true;

	return physWepSelect(event, getCurShip());
}

bool GuiCampaignLoadoutMenu::onUseShip(const SEvent& event)
{
	if (event.GUIEvent.EventType != EGET_BUTTON_CLICKED) return true;
	campaign->getPlayer()->assignedShip = getCurShip();
	getCurShip()->inUseBy = campaign->getPlayer();
	displayShip(getCurShip());
	return false;
}

bool GuiCampaignLoadoutMenu::onReload(const SEvent& event)
{
	if (event.GUIEvent.EventType != EGET_BUTTON_CLICKED) return true;

	if (campaign->getAmmo() == 0) {
		guiController->setOkPopup("", "You don't have the ammunition to reload anything!");
		guiController->showOkPopup();
		return false;
	}
	u32 id = (u32)event.GUIEvent.Caller->getID();
	WeaponInfoComponent& wep = getCurShip()->weps[id]->wep;
	s32 clipsToReload = (wep.maxAmmunition - wep.ammunition) / wep.maxClip;
	if (clipsToReload == 0) {
		guiController->setOkPopup("", "This weapon is already full on ammo.");
		guiController->showOkPopup();
		return false;
	}
	if (campaign->getAmmo() < clipsToReload) {
		u32 reloadedAmmo = clipsToReload * wep.maxClip;
		std::string txt = "You don't have the ammo to reload this gun completely. A partial reload will have to do. \n \n Ammo reloaded: " + std::to_string(reloadedAmmo);
		wep.ammunition += reloadedAmmo;
		campaign->removeAmmo(clipsToReload);
		guiController->setOkPopup("", txt);
		guiController->showOkPopup();
		displayCarrierInfo();
		return false;
	}

	wep.ammunition = wep.maxAmmunition;
	campaign->removeAmmo(clipsToReload);
	displayCarrierInfo();
	return false;
}
bool GuiCampaignLoadoutMenu::onRepair(const SEvent& event)
{
	if (event.GUIEvent.EventType != EGET_BUTTON_CLICKED) return true;

	if (campaign->getSupplies() == 0) {
		guiController->setOkPopup("", "You don't have enough resources to repair anything!");
		guiController->showOkPopup();
		return false;
	}
	ShipInstance* ship = getCurShip();
	f32 healthToRepair = ship->hp.maxHealth - ship->hp.health;
	if (healthToRepair == 0) {
		guiController->setOkPopup("", "This ship is already fully repaired.");
		guiController->showOkPopup();
		return false;
	}
	if (campaign->getSupplies() < healthToRepair) {
		std::string txt = "You don't have the resources to repair this ship completely. A partial repair will have to be enough. \n \n Damage repaired: " + fprecis(campaign->getSupplies(), 5);
		ship->hp.health += campaign->removeSupplies(healthToRepair);
		guiController->setOkPopup("", txt);
		guiController->showOkPopup();
		displayCarrierInfo();
		return false;
	}
	campaign->removeSupplies(healthToRepair);
	ship->hp.health = ship->hp.maxHealth;
	displayShip(ship);
	displayCarrierInfo();
	return false;
}