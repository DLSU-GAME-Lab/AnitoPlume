#pragma once
#include "GUIScreen.hpp"

class PlaybackScreen : public GUIScreen
{
private:
	unsigned int pauseIcon;
	unsigned int playIcon;
	unsigned int resetIcon;

protected:
	PlaybackScreen();
	~PlaybackScreen();

	virtual void drawGUI() override;

	friend class GUIManager;
};