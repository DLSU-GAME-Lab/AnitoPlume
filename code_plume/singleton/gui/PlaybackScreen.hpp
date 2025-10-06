#pragma once
#include "GUIScreen.hpp"

class PlaybackScreen : public GUIScreen
{
protected:
	PlaybackScreen();
	~PlaybackScreen();

	virtual void drawGUI() override;

	friend class GUIManager;
};