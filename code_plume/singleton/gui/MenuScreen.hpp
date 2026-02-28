#pragma once

#include"GUIScreen.hpp"
#include <string>

class MenuScreen : GUIScreen
{
protected:
	MenuScreen();
	~MenuScreen();

	void drawGUI() override;

private:
	void showFile();
	void showEdit();
	void showView();
	void showWindow();
	void showHelp();
	void showAbout();

	friend class GUIManager;
};