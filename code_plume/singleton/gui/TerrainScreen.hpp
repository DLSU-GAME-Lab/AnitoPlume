#pragma once
#include "GUIScreen.hpp"

class TerrainScreen : GUIScreen
{
private:
	unsigned int shader;
	unsigned int currentTexID;
	unsigned int currentNormID;
	unsigned int textureID[5];
	unsigned int normalID[5];

	int currentTex;

protected:
	TerrainScreen();
	~TerrainScreen();

	void drawGUI() override;

	friend class GUIManager;
};