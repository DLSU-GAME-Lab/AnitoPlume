#pragma once
#include "GUIScreen.hpp"
#include <vector>


class SimulatorInputScreen : public GUIScreen
{
private:
    const int alt_min = 0, alt_max = 10000; //meters
    const int wind_min = 0, wind_max = 200; // km/h
    const int angle_min = 0, angle_max = 360; // degrees

    const float indent_width = 5;
    const float child_width = 380;

    const float indent_w = 29;
    const float slider_width = 25;
    const float plot_width = 330;
    const float plot_height = 100;
    const bool show_VEI = false;

    //Wind settings ui
    float max_altitude;
    float altitude_step;

    std::vector<bool> erupt_on_play;
    std::vector<int> deg_angle;
    bool all_angles;
    bool is_wind;
    int selected;
    int wind_alt;
    float linear_wind_base;
    int plume_index;
    
	SimulatorInputScreen();
	~SimulatorInputScreen();

	virtual void drawGUI() override;

    void showWindSettings();
    void showEruptionParameters();

	friend class GUIManager;

public:
    void resetEruptOnPlay();

};