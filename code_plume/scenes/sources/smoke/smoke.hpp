#pragma once

#include "scenes/sources/smoke/Plume.hpp"
#include "scenes/sources/smoke/terrain_loader/terrain_loader.hpp"
#include "scenes/sources/smoke/tooltip_loader/tooltip_loader.hpp"
#include "scenes/sources/smoke/landmark_loader/landmark_loader.hpp"
#include "singleton/gui/SimulatorInputScreen.hpp"
#include "singleton/gui/DisplaySettingsScreen.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <ctime>
#include <cstdio>
#include <chrono>
#include <thread>
#include <future>

struct scene_model
{
    size_t frame_replay;

    // Trackers
    float avg_wind_dir_degrees;
    double fU0;
    double fRho0;
    double fR0;
    double fZ0;

    // Meshes
    GLuint smoke_texture;
    GLuint decal;

    vcl::mesh_drawable terrain_display;
    vcl::mesh_drawable* skysphere_display;
    vcl::mesh_drawable* torus_display;
    vcl::mesh_drawable* quad_display;
    vcl::mesh_drawable* sphere_display;
    vcl::mesh_drawable* subspheres_display;

    // PLUMES
    //std::vector<Plume> plumes;
    const char* vent_names[5];
    std::vector<EruptionParams> erupt_params;
    vcl::vec3 vent_positions[5];
    unsigned short vent_index;

    std::vector<float> sphere_lifetime;
    vcl::vec3 last_cam_pos;

    SimulatorInputScreen* sim_input_screen;
    DisplaySettingsScreen* display_screen;
    terrain_structure terrain_struct;
    terrain_loader t_loader;
    tooltip_loader tip_loader;
    landmark_loader mark_loader;

public:
    // General functions
    void setup_resources();
    void setup_data();
    void setup_plume_params();

    void update();

    void frame_draw();
    void display();

private:
    void display_smoke_layers(Plume* plume);
    void display_billboards(Plume* plume);
    void display_free_spheres(Plume* plume);
    void display_spheres_with_subspheres(Plume* plume);
    void display_subspheres(Plume* plume);
    void display_falling_spheres(Plume* plume);
    void display_falling_spheres_buffers(Plume* plume);

    void setup_terrain_preemptive();

    void stop();
    void play();
    void pause();

public:
    // Input
    void keyboard_input(GLFWwindow* window, int key, int scancode, int action, int mods);
};


