#pragma once

#ifndef GUIMAINMENU_H
#define GUIMAINMENU_H

#include "BaseHeader.h"
#include "GuiDialog.h"

//forward declarations of things needed
class GameStateController;
class GuiController;

//enum to handle all the different button IDs for this dialog.
enum MAIN_MENU_BUTTONS
{
	GUI_START_GAME,
	GUI_SETTINGS,
	GUI_QUIT_GAME
};

class GuiMainMenu : public GuiDialog
{
	public:
		GuiMainMenu(GuiController* controller) : GuiDialog(controller), startGame(0), settings(0), quitGame(0), root(0) {}

		virtual void init();

		//show() and hide() just set the root node to visible or not, thus showing or hiding the entire menu.
		virtual void show() { root->setVisible(true); isVisible = true; }
		virtual void hide() { root->setVisible(false); isVisible = false; }
		virtual void handleEvent(const SEvent& event);
	private:
		IGUIStaticText* root;
		IGUIButton* startGame;
		IGUIButton* settings;
		IGUIButton* quitGame;
};


#endif
