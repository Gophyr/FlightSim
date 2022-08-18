#include "GuiCampaignMenu.h"
#include "GameController.h"
#include "GameStateController.h"
#include "GuiController.h"

void GuiCampaignMenu::init()
{
	if (root) root->remove();

	root = guienv->addImage(rect<s32>(position2di(0, 0), baseSize));
	IGUIImage* img = (IGUIImage*)root;
	img->setImage(driver->getTexture("ui/starbg.png"));
	scaleAlign(img);

	hud.HUDimg = guienv->addImage(rect<s32>(position2di(0, 0), baseSize), root);
	hud.HUDimg->setImage(driver->getTexture("ui/campaignbase.png"));
	scaleAlign(hud.HUDimg);

	hud.orbiting = guienv->addImage(rect<s32>(position2di(150, 250), dimension2du(96, 96)), root);
	hud.orbiting->setImage(driver->getTexture("ui/planet.png"));
	scaleAlign(hud.orbiting);

	hud.shipSprite = guienv->addImage(rect<s32>(position2di(300, 280), dimension2du(46, 17)), root);
	hud.shipSprite->setImage(driver->getTexture("ui/shipsprite.png"));
	scaleAlign(hud.shipSprite);
	hud.advance = guienv->addButton(rect<s32>(position2di(275, 300), dimension2du(100, 25)), root, -1, L"Advance", L"Move to the next scenario.");
	setHoloButton(hud.advance, true);

	for (u32 i = 0; i < NUM_SCENARIO_OPTIONS; ++i) {
		s32 startX=250;
		s32 startY=435;
		s32 buf = 4;
		dimension2du size(110, 30);
		IGUIButton* button = guienv->addButton(rect<s32>(position2di(startX + (size.Width * i) + (buf * i), startY), size), root, (s32)i, L"", L"Possible scenario");
		setHoloButton(button, true);
		hud.scenarioSelects[i] = button;
		guiController->setCallback(button, std::bind(&GuiCampaignMenu::onShowScenarioInfo, this, std::placeholders::_1));
	}

	hud.toMenu = guienv->addButton(rect<s32>(position2di(0, 0), dimension2du(90, 55)), hud.HUDimg, CAMPAIGN_TO_MENU, L"Menu", L"Back out for now.");
	setHoloButton(hud.toMenu);
	guiController->setCallback(hud.toMenu, std::bind(&GuiCampaignMenu::onMenu, this, std::placeholders::_1));
	hud.info = guienv->addStaticText(L"", rect<s32>(position2di(0, 160), dimension2du(90, 275)), false, true, hud.HUDimg);
	setUIText(hud.info);

	hud.constructionButton = guienv->addButton(rect<s32>(position2di(0, 512), dimension2du(185, 28)), hud.HUDimg, -1, L"Construction Bay", L"Build and repair your armory.");
	setHoloButton(hud.constructionButton);

	scenariohud.areadesc = guienv->addImage(rect<s32>(position2di(960, 114), dimension2du(305, 311)), root);
	scenariohud.areadesc->setImage(driver->getTexture("ui/areadesc.png"));
	scaleAlign(scenariohud.areadesc);
	scenariohud.name = guienv->addStaticText(L"", rect<s32>(position2di(15, 20), dimension2du(280, 20)), false, true, scenariohud.areadesc);
	scenariohud.desc = guienv->addStaticText(L"", rect<s32>(position2di(35, 45), dimension2du(250, 250)), false, true, scenariohud.areadesc);
	setUIText(scenariohud.name);
	setUIText(scenariohud.desc);
	scenariohud.launch = guienv->addImage(rect<s32>(position2di(362, 4), dimension2du(236, 51)), root);
	scenariohud.launch->setImage(driver->getTexture("ui/launch.png"));
	scaleAlign(scenariohud.launch);
	scenariohud.launchButton = guienv->addButton(rect<s32>(position2di(54, 6), dimension2du(135, 20)), scenariohud.launch, -1, L"Start", L"Shoot mans!");
	setHoloButton(scenariohud.launchButton);
	guiController->setCallback(scenariohud.launchButton, std::bind(&GuiCampaignMenu::onStart, this, std::placeholders::_1));

	hud.sectorHeader = guienv->addImage(rect<s32>(position2di(290, 0), dimension2du(380, 55)), root);
	hud.sectorHeader->setImage(driver->getTexture("ui/header.png"));
	scaleAlign(hud.sectorHeader);

	loadout.img = guienv->addImage(rect<s32>(position2di(286, 470), dimension2du(388,359)), root);
	loadout.img->setImage(driver->getTexture("ui/loadout.png"));
	scaleAlign(loadout.img);
	
	loadout.button = guienv->addButton(rect<s32>(position2di(67, 25), dimension2du(260, 40)), loadout.img, CAMPAIGN_LOADOUT, L"Loadouts", L"View loadouts.");
	setHoloButton(loadout.button);
	loadout.toLoadoutMenu = guienv->addButton(rect<s32>(position2di(67, 70), dimension2du(260, 40)), loadout.img, -1, L"Set Loadout", L"Set your ship and loadout.");
	setHoloButton(loadout.toLoadoutMenu);
	loadout.wingmanButton = guienv->addButton(rect<s32>(position2di(67, 115), dimension2du(260, 40)), loadout.img, -1, L"Set Wingmen", L"Select your wingmen.");
	setHoloButton(loadout.wingmanButton);

	guiController->setCallback(loadout.button, std::bind(&GuiCampaignMenu::onLoadout, this, std::placeholders::_1));
	guiController->setCallback(loadout.toLoadoutMenu, std::bind(&GuiCampaignMenu::onLoadoutMenuSelect, this, std::placeholders::_1));
	guiController->setCallback(hud.advance, std::bind(&GuiCampaignMenu::onAdvance, this, std::placeholders::_1));
	guiController->setCallback(loadout.wingmanButton, std::bind(&GuiCampaignMenu::onWingman, this, std::placeholders::_1));
	guiController->setAnimationCallback(loadout.button, std::bind(&GuiCampaignMenu::moveLoadout, this, std::placeholders::_1));
	guiController->setAnimationCallback(scenariohud.launch, std::bind(&GuiCampaignMenu::moveScenarioInfo, this, std::placeholders::_1));

	guiController->setAnimationCallback(hud.advance, std::bind(&GuiCampaignMenu::moveAdvance, this, std::placeholders::_1));
	hide();
}

void GuiCampaignMenu::show()
{
	root->setRelativePosition(rect<s32>(position2di(0, 0), driver->getScreenSize()));
	root->setVisible(true);

	for (u32 i = 0; i < NUM_SCENARIO_OPTIONS; ++i) {
		Scenario scen = campaign->getSector()->getScenario(i);
		std::wstring title = wstr(scen.location);
		hud.scenarioSelects[i]->setText(title.c_str());
	}
	std::string info = "Ship Information \n \n";
	info += "Total Ammo: \n" + std::to_string(campaign->getAmmo());
	info += "\n Supplies: \n" + fprecis(campaign->getSupplies(), 5);
	hud.info->setText(wstr(info).c_str());
	stateController->inCampaign = true;

	if (campaign->getSector()->hasMoved()) {
		for (u32 i = 0; i < NUM_SCENARIO_OPTIONS; ++i) {
			hud.scenarioSelects[i]->setVisible(true);
		}
		hud.advance->setVisible(false);
	}
	else {
		for (u32 i = 0; i < NUM_SCENARIO_OPTIONS; ++i) {
			hud.scenarioSelects[i]->setVisible(false);
		}
		hud.advance->setVisible(true);
	}

	s32 baseShipX = (s32)((300.f / 960.f) * root->getRelativePosition().getWidth());
	s32 baseAdvanceX = (s32)((275.f / 960.f) * root->getRelativePosition().getWidth());
	s32 move = (s32)((80.f / 960.f) * root->getRelativePosition().getWidth());

	hud.advance->setRelativePosition(position2di(baseAdvanceX + campaign->getSector()->getEncounterNum() *move, hud.advance->getRelativePosition().UpperLeftCorner.Y));
	hud.shipSprite->setRelativePosition(position2di(baseShipX + campaign->getSector()->getEncounterNum() * move, hud.shipSprite->getRelativePosition().UpperLeftCorner.Y));
}

bool GuiCampaignMenu::onStart(const SEvent& event)
{

	if (event.GUIEvent.EventType != EGET_BUTTON_CLICKED) return true;
	guiController->callAnimation(scenariohud.launch);
	stateController->setState(GAME_RUNNING);
	audioDriver->playMusic("combat_default.ogg");
	//stateController->changeMusic(assets->getSoundAsset("battleMusic"));
	return false;
}

bool GuiCampaignMenu::onMenu(const SEvent& event)
{
	if (event.GUIEvent.EventType != EGET_BUTTON_CLICKED) return true;
	guiController->setActiveDialog(GUI_MAIN_MENU);
	stateController->inCampaign = false;
	audioDriver->playMusic("main_menu.ogg");
	//stateController->changeMusic(assets->getSoundAsset("menuMusic"));
	return false;
}

bool GuiCampaignMenu::onWingman(const SEvent& event)
{
	if (event.GUIEvent.EventType != EGET_BUTTON_CLICKED) return true;
	guiController->setActiveDialog(GUI_WINGMAN_MENU);
	return false;
}

bool GuiCampaignMenu::onLoadout(const SEvent& event)
{
	if (event.GUIEvent.EventType != EGET_BUTTON_CLICKED) return true;
	loadout.timer = 0;
	loadout.isOpen = !loadout.isOpen;
	guiController->callAnimation(loadout.button);
	return false;
}

bool GuiCampaignMenu::moveLoadout(f32 dt)
{
	rect<s32> relpos = loadout.img->getRelativePosition();
	position2di center = relpos.getCenter();

	position2di pos(center.X - relpos.getWidth() / 2, center.Y - relpos.getHeight() / 2);
	rect<s32> screen = root->getAbsolutePosition();

	s32 openHeight = screen.getHeight() - relpos.getHeight();
	s32 closeHeight = screen.getHeight() - relpos.getHeight() / 5;

	vector2di open(pos.X, openHeight);
	vector2di close(pos.X, closeHeight);
	f32 animTime = .1f;

	if (loadout.isOpen) {
		return smoothGuiMove(loadout.img, animTime, loadout.timer, close, open, dt);
	}
	else {
		return smoothGuiMove(loadout.img, animTime, loadout.timer, open, close, dt);
	}
}

bool GuiCampaignMenu::onLoadoutMenuSelect(const SEvent& event)
{
	if (event.GUIEvent.EventType != EGET_BUTTON_CLICKED) return true;
	guiController->setActiveDialog(GUI_CAMPAIGN_LOADOUT_MENU);
	return false;
}

bool GuiCampaignMenu::onShowScenarioInfo(const SEvent& event)
{
	if (event.GUIEvent.EventType != EGET_BUTTON_CLICKED) return true;
	s32 id = event.GUIEvent.Caller->getID();
	if (!sectorInfoShowing || scenariohud.showing == id) {
		guiController->callAnimation(scenariohud.launch);
	}
	Scenario scen = campaign->getSector()->getScenario(id);
	std::wstring name = wstr(scen.location);
	std::string desc = scen.description;
	desc += "\n \n";
	desc += "Detection chance: " + std::to_string(scen.detectionChance) + "%";
	scenariohud.name->setText(name.c_str());
	scenariohud.desc->setText(wstr(desc).c_str());
	scenariohud.showing = id;
	campaign->getSector()->selectCurrentScenario(id);
	return false;
}

bool GuiCampaignMenu::moveScenarioInfo(f32 dt)
{
	f32 launchRatioOpen = 4.f / 540.f;
	f32 launchRatioClosed = 55.f / 540.f;
	f32 areaRatioOpen = 305.f / 960.f;

	rect<s32> launch = scenariohud.launch->getRelativePosition();
	rect<s32> area = scenariohud.areadesc->getRelativePosition();
	rect<s32> screen = root->getAbsolutePosition();
	position2di launchPos(launch.UpperLeftCorner);
	position2di areaPos(area.UpperLeftCorner);

	vector2di launchOpen(launchPos.X, (s32)(screen.getHeight() * launchRatioOpen));
	vector2di launchClosed(launchPos.X, (s32)(screen.getHeight() * launchRatioClosed));
	vector2di areaOpen(screen.getWidth() - (s32)(screen.getWidth() * areaRatioOpen), areaPos.Y);
	vector2di areaClosed(screen.getWidth(), areaPos.Y);

	bool move;
	if (sectorInfoShowing) {
		smoothGuiMove(scenariohud.areadesc, .1f, scenariohud.timer1, areaOpen, areaClosed, dt);
		move = smoothGuiMove(scenariohud.launch, .1f, scenariohud.timer2, launchClosed, launchOpen, dt);
	}
	else {
		smoothGuiMove(scenariohud.areadesc, .1f, scenariohud.timer1, areaClosed, areaOpen, dt);
		move = smoothGuiMove(scenariohud.launch, .1f, scenariohud.timer2, launchOpen, launchClosed, dt);
	}
	if (!move) {
		sectorInfoShowing = !sectorInfoShowing;
	}
	return move;
	//return false;
}

bool GuiCampaignMenu::onAdvance(const SEvent& event)
{
	if (event.GUIEvent.EventType != EGET_BUTTON_CLICKED) return true;
	guiController->setYesNoPopup("Confirm Movement",
		"Are you ready to move to the next scenario?",
		std::bind(&GuiCampaignMenu::advanceConfirm, this, std::placeholders::_1));
	guiController->showYesNoPopup();
	return false;
}
bool GuiCampaignMenu::advanceConfirm(const SEvent& event)
{
	if (event.GUIEvent.EventType != EGET_BUTTON_CLICKED) return true;
	campaign->advance();

	for (u32 i = 0; i < NUM_SCENARIO_OPTIONS; ++i) {
		hud.scenarioSelects[i]->setVisible(true);
	}
	guiController->callAnimation(hud.advance);
	hud.advance->setVisible(false);
	return false;
}
bool GuiCampaignMenu::moveAdvance(f32 dt)
{
	s32 baseShipX = (s32)((275.f / 960.f) * root->getRelativePosition().getWidth());
	s32 move = (s32)((80.f / 960.f) * root->getRelativePosition().getWidth());
	position2di shipMove = position2di(baseShipX + campaign->getSector()->getEncounterNum() * move, hud.shipSprite->getRelativePosition().UpperLeftCorner.Y);
	position2di shipOld = shipMove;
	shipOld.X -= move;

	return smoothGuiMove(hud.shipSprite, .2f, hud.timer1, shipOld, shipMove, dt);
}