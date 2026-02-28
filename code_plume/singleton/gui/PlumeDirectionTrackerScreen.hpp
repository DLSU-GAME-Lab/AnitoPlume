#pragma once
#include "GUIScreen.hpp"
#include <vector>

class PlumeDirectionTrackerScreen : public GUIScreen
{
private:
	const float lineLen = 108.0f;
	const float lineThickness = 2.0f;
	const ImU32 red = IM_COL32(240, 0, 40, 255);
	const ImU32 alphaRed = IM_COL32(240, 0, 40, 100);

	const float imgSize = 360.0f;
	const float halfSize = imgSize / 2.0f;
	const float ratio = 360.0f;
	const float xOffset = 4.0f;
	const float yOffset = -6.0f;

	const int steps = 10;
	const float altitudeStep = 1000.0f;

	unsigned int dangerZoneImage;
	bool windVectorsEnabled = true;
	bool layeredViewEnabled = true;
	bool windConeEnabled = false;

protected:
	PlumeDirectionTrackerScreen();
	~PlumeDirectionTrackerScreen();

	virtual void drawGUI() override;

	void showAffectedAreas();

	friend class GUIManager;
};