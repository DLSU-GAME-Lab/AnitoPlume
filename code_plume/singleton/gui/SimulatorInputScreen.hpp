#pragma once
#include "GUIScreen.hpp"
#include <vector>


class SimulatorInputScreen : public GUIScreen
{
private:
    static const int wind_size = 6;

    const int alt_min = 0, alt_max = 10000;
    const int wind_min = 0, wind_max = 200;
    const int angle_min = 0, angle_max = 360;

    const float indent_w = 29;
    const float slider_width = 25;
    const float plot_width = 330;
    const float plot_height = 100;

    std::vector<int> deg_angle; // UI wind angles
    bool all_angles; // UI toggle

	SimulatorInputScreen();
	~SimulatorInputScreen();

	virtual void drawGUI() override;

	friend class GUIManager;
};