#pragma once

#include"GUIScreen.hpp"
#include <string>

class CameraSettingsScreen : GUIScreen
{
protected:
	CameraSettingsScreen();
	~CameraSettingsScreen();

	void drawGUI() override;

	friend class GUIManager;
};