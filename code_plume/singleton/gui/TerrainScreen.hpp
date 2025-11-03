#pragma once
#include "GUIScreen.hpp"

class TerrainScreen : GUIScreen
{
private:
	unsigned int currentTexID = 0;
	unsigned int currentNormID = 0;
	int currentTex = 0;

public:
	void initialize();

	unsigned int getCurrentTex() const;
	unsigned int getCurrentNormTex() const;

protected:
	TerrainScreen();
	~TerrainScreen();

	void drawGUI() override;

	friend class GUIManager;
};