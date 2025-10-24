#pragma once
#include "GUIScreen.hpp"

class ProfilerScreen : public GUIScreen
{
private:
	unsigned int pauseIcon;
	unsigned int playIcon;
	unsigned int resetIcon;

protected:
	ProfilerScreen();
	~ProfilerScreen();

	virtual void drawGUI() override;

	friend class GUIManager;
};