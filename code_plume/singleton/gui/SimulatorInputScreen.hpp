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

    //Wind settings ui
    float max_altitude;
    float altitude_step;
    int altitude_size;

    ////wind stuff
    //std::vector<int> wind_altitudes;
    //std::vector<wind_structure> winds;
    //float linear_wind_base;
    //
    std::vector<int> deg_angle;
    bool all_angles;
    bool is_wind;
    int selected;
    int wind_alt;
    
    // Trackers
    float sim_time;
    float avg_wind_dir_degrees;
    double fU0;
    double fRho0;
    double fR0;
    double fZ0;

    // Display settings
    bool display_smoke_layers;
    bool display_free_spheres;
    bool display_subspheres;
    bool display_spheres_with_subspheres;
    bool display_billboards;
    bool display_tooltips;

	SimulatorInputScreen();
	~SimulatorInputScreen();

	virtual void drawGUI() override;

    void show_display_settings();
    void show_wind_settings();
    void show_eruption_parameters();

	friend class GUIManager;
};