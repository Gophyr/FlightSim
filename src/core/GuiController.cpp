#include "GuiController.h"
#include "GameStateController.h"
#include <iostream>

GuiController::GuiController()
{
	activeDialog = 0;

	gvReader tauntReader;
	tauntReader.read("attributes/taunts.txt");
	for (std::string line : tauntReader.lines) {
		taunts.push_back(std::wstring(line.begin(), line.end()));
	}
	dimension2du baseSize(960, 540);
	u32 horizpos = (baseSize.Width - 512) / 2;
	u32 vertpos = (baseSize.Height - 512) / 2;
	popup.bg = guienv->addImage(rect<s32>(position2di(horizpos, vertpos), dimension2du(512, 512)));
	popup.bg->setImage(driver->getTexture("ui/popup.png"));
	popup.button = guienv->addButton(rect<s32>(position2di(192, 448), dimension2du(128, 64)), popup.bg, 2500, L"Got it");
	popup.title = guienv->addStaticText(L"Heads up", rect<s32>(position2di(64, 24), dimension2du(384, 24)), false, true, popup.bg);
	popup.body = guienv->addStaticText(L"", rect<s32>(position2di(64, 16), dimension2du(384, 400)), false, true, popup.bg);
	scaleAlign(popup.bg);
	popup.bg->setScaleImage(true);

	popup.bg->setVisible(false);
	setMetalButton(popup.button);
	setUIText(popup.title);
	setUIText(popup.body);
	setCallback(popup.button, std::bind(&GuiController::hidePopup, this, std::placeholders::_1));
}

void GuiController::init()
{
	//All menus get initialized here. Don't delete them.
	//If you've just added a new menu, initialize it here.
	menus.menuDialogs[GUI_MAIN_MENU] = new GuiMainMenu;
	menus.menuDialogs[GUI_MAIN_MENU]->init();
	menus.menuDialogs[GUI_PAUSE_MENU] = new GuiPauseMenu;
	menus.menuDialogs[GUI_PAUSE_MENU]->init();
	menus.menuDialogs[GUI_DEATH_MENU] = new GuiDeathMenu;
	menus.menuDialogs[GUI_DEATH_MENU]->init();
	menus.menuDialogs[GUI_SETTINGS_MENU] = new GuiSettingsMenu;
	menus.menuDialogs[GUI_SETTINGS_MENU]->init();
	menus.menuDialogs[GUI_LOADOUT_MENU] = new GuiLoadoutMenu;
	menus.menuDialogs[GUI_LOADOUT_MENU]->init();
	menus.menuDialogs[GUI_CAMPAIGN_MENU] = new GuiCampaignMenu;
	menus.menuDialogs[GUI_CAMPAIGN_MENU]->init();
	//set up death menu
	setActiveDialog(GUI_MAIN_MENU);
	//activeDialog = menus.menuDialogs[GUI_MAIN_MENU];
	//activeDialog->show();
	//default main menu
}

void GuiController::setPopup(std::string title, std::string body, std::string button)
{
	popup.title->setText(wstr(title).c_str());
	popup.body->setText(wstr(body).c_str());
	popup.button->setText(wstr(button).c_str());
}
void GuiController::showPopup()
{
	dimension2du screenSize = driver->getScreenSize();
	u32 horizpos = (screenSize.Width - 512) / 2;
	u32 vertpos = (screenSize.Height - 512) / 2;

	popup.bg->setRelativePosition(position2di(horizpos, vertpos));
	popup.bg->setVisible(true);
	guienv->getRootGUIElement()->bringToFront(popup.bg);
	popupActive = true;
}
bool GuiController::hidePopup(const SEvent& event)
{
	if (event.GUIEvent.EventType != EGET_BUTTON_CLICKED || event.GUIEvent.Caller != popup.button) return true;
	popup.bg->setVisible(false);
	popupActive = false;
	return false;
}

void GuiController::setAnimationCallback(IGUIElement* elem, AnimationCallback cb)
{
	animationCallbacks[elem] = cb;
}
void GuiController::setOpenAnimationCallback(GuiDialog* dialog, AnimationCallback cb)
{
	openAnimationCallbacks[dialog] = cb;
}
void GuiController::setCloseAnimationCallback(GuiDialog* dialog, AnimationCallback cb)
{
	closeAnimationCallbacks[dialog] = cb;
}

void GuiController::callCloseAnimation(GuiDialog* dialog)
{
	if (closeAnimationCallbacks.find(dialog) != closeAnimationCallbacks.end()) {
		currentAnimation = closeAnimationCallbacks[dialog];
		playingAnimation = true;
	}
}
void GuiController::callOpenAnimation(GuiDialog* dialog)
{
	if (openAnimationCallbacks.find(dialog) != openAnimationCallbacks.end()) {
		currentAnimation = openAnimationCallbacks[dialog];
		playingAnimation = true;
	}
}
void GuiController::callAnimation(IGUIElement* elem)
{
	if (animationCallbacks.find(elem) != animationCallbacks.end()) {
		currentAnimation = animationCallbacks[elem];
		playingAnimation = true;
	}
}

std::wstring GuiController::getTaunt()
{
	return taunts[std::rand() % taunts.size()]; //Pulls out a random taunt to mess with the player
}

void GuiController::close()
{
	if (activeDialog) {
		activeDialog->hide();
		activeDialog = nullptr;
	} //Doesn't actually delete anything; that's what menuCleanup is for (and guienv->clear() is called elsewhere to actually remove elements)
}

bool GuiController::OnEvent(const SEvent& event)
{
	//Hurls the event to the active dialog.
	if (activeDialog && event.EventType == EET_GUI_EVENT && callbacks.find(event.GUIEvent.Caller) != callbacks.end()) {
		if(!popupActive) return callbacks[event.GUIEvent.Caller](event);

		return hidePopup(event);
	}
	return true;
}

void GuiController::setCallback(IGUIElement* elem, GuiCallback callback)
{
	callbacks[elem] = callback;
}

void GuiController::removeCallback(IGUIElement* elem)
{
	callbacks.erase(elem);
}

void GuiController::update()
{
	u32 now = device->getTimer()->getTime();
	f32 delta = (f32)(now - then) / 1000.f;
	if (delta > .25) { //If the delta is too big, it's .25.
		delta = .25;
	}
	then = now;
	accumulator += delta;
	while (accumulator >= dt) {
		if (playingAnimation) {
			playingAnimation = currentAnimation(dt);
		}

		if (!playingAnimation && switchMenusCalled) {
			if (activeDialog) activeDialog->hide();

			activeDialog = menus.menuDialogs[menuToSwitch];
			activeDialog->show();
			callOpenAnimation(activeDialog);
			guienv->getRootGUIElement()->bringToFront(activeDialog->getRoot());
			switchMenusCalled = false;
		}

		t += dt;
		accumulator -= dt;
	}
}

//If you've just added a new menu, go make sure that you added it as a menu type in MenuData.h
void GuiController::setActiveDialog(MENU_TYPE menu)
{
	if(activeDialog) callCloseAnimation(activeDialog);
	menuToSwitch = menu;
	switchMenusCalled = true;
}