#pragma once

#include "scenes/sources/smoke/Plume.hpp"
#include "scenes/sources/smoke/terrain_loader/terrain_loader.hpp"
#include "scenes/sources/smoke/tooltip_loader/tooltip_loader.hpp"
#include "scenes/sources/smoke/landmark_loader/landmark_loader.hpp"
#include "scenes/sources/smoke/direction_tracker/direction_tracker.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <ctime>
#include <cstdio>
#include <chrono>
#include <thread>
#include <future>

// User parameters available in the GUI
struct gui_parameters
{
    bool display_smoke_layers;
    bool display_free_spheres;
    bool display_subspheres;
    bool display_spheres_with_subspheres;
    bool display_billboards;
    bool display_tooltips;
};

enum class engine_state {stopped, playing, paused};

struct scene_model : scene_base
{
    unsigned int frame_count;
    vcl::timer_event timer;
    float dt;
    float t_step;
    bool debug_mode;
    bool replay;
    size_t frame_replay;
    bool export_data;
    engine_state state;

    // Trackers
    float sim_time;
    float avg_wind_dir_degrees;

    // Meshes

    vcl::mesh_drawable terrain_display;
    GLuint smoke_texture;
    GLuint pauseIcon;
    GLuint playIcon;
    GLuint resetIcon;
    GLuint decal;

    vcl::mesh_drawable subspheres_display;

    std::vector<int> wind_altitudes;
    std::vector<wind_structure> winds;
    float linear_wind_base;
    bool is_wind;
    int selected;
    int wind_alt;
    std::vector<int> deg_angle; // UI wind angles
    bool all_angles; // UI toggle



    //Wind settings ui
    float max_altitude;
    float altitude_step;
    int altitude_size;

    int direction_tracker_step_size;
    float direction_tracker_step;

    // PLUMES
    //std::vector<Plume> plumes;
    std::vector<vcl::vec3> vent_positions;
    unsigned short vent_index;

    std::vector<float> sphere_lifetime;


    terrain_structure terrain_struct;
    terrain_loader t_loader;
    direction_tracker direction_tracker;
    tooltip_loader tip_loader;
    landmark_loader mark_loader;

    // General functions
    void setup_data(scene_structure& scene, gui_structure& gui);
    void setup_plume_params();

    void update();

    void frame_draw(scene_structure& scene, gui_structure& gui);
    void display(scene_structure& scene);

    void display_smoke_layers(Plume& plume);
    void display_billboards(Plume& plume);
    void display_free_spheres(Plume& plume);
    void display_spheres_with_subspheres(Plume& plume);
    void display_subspheres(Plume& plume);
    void display_falling_spheres(Plume& plume);
    void display_falling_spheres_buffers(Plume& plume);

    void reset_simulation();
    void setup_terrain_preemptive();

    // Smoke layer computation
    vcl::vec3 compute_wind_vector(float height);
    // Input
    void keyboard_input(scene_structure& scene, GLFWwindow* window, int key, int scancode, int action, int mods);

    void set_gui(gui_structure& gui);
    void show_display_settings();
    void show_wind_settings();
    void show_eruption_parameters();

    void set_gui_playback(gui_structure& gui);
    void set_gui_profiler(gui_structure& gui);

    gui_parameters gui_param;
};


