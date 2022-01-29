#include "GuiPauseMenu.h"
#include "GuiController.h"
#include "GameStateController.h"

void GuiPauseMenu::init()
{
	dimension2du size = guiController->driver->getScreenSize();
	u32 verticalSlice = size.Height / 6;
	u32 horizontalPos = size.Width / 3;
	dimension2du buttonSize(horizontalPos, verticalSlice);
	root = guiController->guienv->addStaticText(L"PAUSE MENU", rect<s32>(position2di(0, 0), size));
	resumeGame = guiController->guienv->addButton(rect<s32>(position2di(horizontalPos, 32), buttonSize), root, GUI_PAUSE_RESUME, L"Resume Game", L"Get back in there!");
	pauseSettings = guiController->guienv->addButton(rect<s32>(position2di(horizontalPos, 32 * 2 + verticalSlice), buttonSize), root, GUI_PAUSE_SETTINGS, L"Settings", L"What, is your sensitivity too low?");
	exitToMenus = guiController->guienv->addButton(rect<s32>(position2di(horizontalPos, 32 * 3 + verticalSlice * 2), buttonSize), root, GUI_PAUSE_EXIT_TO_MENU, L"Exit to Main Menu", L"Run, coward!");
	root->setVisible(false);
}

void GuiPauseMenu::handleEvent(const SEvent& event)
{
	switch (event.EventType) {
	case EET_MOUSE_INPUT_EVENT:
		break;
	case EET_KEY_INPUT_EVENT:
		break;
	case EET_GUI_EVENT:
		s32 id = event.GUIEvent.Caller->getID();
		if (event.GUIEvent.EventType == EGET_BUTTON_CLICKED) {
			switch (id) {
			case GUI_PAUSE_RESUME:
				guiController->stateController->setState(GAME_RUNNING);
				break;
			case GUI_PAUSE_SETTINGS:
				break;
			case GUI_PAUSE_EXIT_TO_MENU:
				guiController->stateController->setState(GAME_MENUS);
				break;
			}
		}
		break;
	}
}