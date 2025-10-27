#pragma once
#include "GUIScreen.hpp"

class ProfilerScreen : public GUIScreen
{
protected:
	ProfilerScreen();
	~ProfilerScreen();

	virtual void drawGUI() override;

	friend class GUIManager;
};