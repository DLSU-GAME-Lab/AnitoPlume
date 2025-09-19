#pragma once

#include <string>

class GUIManager;

class GUIScreen
{
protected:
	GUIScreen(std::string name);
	~GUIScreen();

	std::string getName();
	virtual void drawGUI() = 0;

	std::string name;
	bool enabled = true;

	friend class GUIManager;
};