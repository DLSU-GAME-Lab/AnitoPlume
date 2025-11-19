#pragma once
#include "GUIScreen.hpp"

class DisplaySettingsScreen : GUIScreen
{
private:
	unsigned int currentTexID = 0;
	unsigned int currentNormID = 0;
	int currentTex = 0;

	bool display_tooltips = true;
	bool display_landmarks = true;
	bool display_billboards = true;
	bool display_smoke_layers = false;
	bool display_free_spheres = false;
	bool display_subspheres = false;
	bool display_spheres_with_subspheres = false;

public:
	void initialize();

	unsigned int getCurrentTex() const;
	unsigned int getCurrentNormTex() const;

	bool getDisplaySmokeLayers() const;
	void setDisplaySmokeLayers(bool display);
	bool getDisplayFreeSpheres() const;
	void setDisplayFreeSpheres(bool display);
	bool getDisplaySubspheres() const;
	void setDisplaySubspheres(bool display);
	bool getDisplaySpheresWithSubspheres() const;
	void setDisplaySpheresWithSubspheres(bool display);
	bool getDisplayBillboards() const;
	void setDisplayBillboards(bool display);
	bool getDisplayTooltips() const;
	void setDisplayTooltips(bool display);
	bool getDisplayLandmarks() const;
	void setDisplayLandmarks(bool display);
protected:
	DisplaySettingsScreen();
	~DisplaySettingsScreen();

	void drawGUI() override;

	friend class GUIManager;
};