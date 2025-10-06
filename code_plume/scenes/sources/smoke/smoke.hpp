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
    vcl::mesh mesh_terrain;

    vcl::mesh_drawable generic_torus_mesh;
    vcl::mesh_drawable generic_sphere_mesh;
    vcl::mesh_drawable layer_mesh;
    vcl::mesh_drawable terrain;
    vcl::mesh_drawable skysphere;
    vcl::mesh_drawable terrain_display;
    vcl::mesh_drawable sphere;
    vcl::mesh_drawable sky_sphere;
    vcl::mesh_drawable quad;
    vcl::curve_drawable sphere_circle;
    vcl::skybox_drawable skybox;
    vcl::vec3 avg_wind_direction;
    GLuint skybox_tex;
    GLuint smoke_texture;
    GLuint pauseIcon;
    GLuint playIcon;
    GLuint resetIcon;
    GLuint decal;

    std::vector<vcl::vec3> samples_subspheres;
    vcl::mesh_drawable subspheres;
    vcl::mesh_drawable subspheres_display;

    std::vector<int> wind_altitudes;
    std::vector<wind_structure> winds;
    float linear_wind_base;
    bool is_wind;
    int selected;
    int wind_alt;
    std::vector<int> deg_angle; // UI wind angles
    bool all_angles; // UI toggle

    // smoke transition animation
    int max_smoke;
    float transition_speed;
    float transition_delay;

    //Wind settings ui
    float max_altitude;
    float altitude_step;
    int altitude_size;

    int direction_tracker_step_size;
    float direction_tracker_step;

    // PLUMES
    std::vector<Plume*> plumes;
    std::vector<vcl::vec3> vent_positions;
    unsigned short vent_index;

    float new_layer_delay;

    std::vector<float> sphere_lifetime;
    std::vector<float> transition_lifetime;

    terrain_structure terrain_struct;
    terrain_loader t_loader;
    direction_tracker direction_tracker;
    tooltip_loader tip_loader;
    landmark_loader mark_loader;

    //// For replay feature
    //std::vector< std::vector<smoke_layer> > smoke_layers_frames;
    //std::vector< std::vector<free_sphere_params> > free_spheres_frames;
    //std::vector< std::vector<subsphere_params> > s2_spheres_frames;
    //std::vector< std::vector<free_sphere_params> > stagnate_spheres_frames;
    //std::vector< std::vector<free_sphere_params> > falling_spheres_frames;
    //std::vector< std::vector< std::vector<free_sphere_params> > > falling_spheres_buffers_frames;

    // Export structures
    std::string const altitude_file = "../output/altitude.txt";
    std::string const plumex_file = "../output/plumex_file.txt";
    std::string const speed_file = "../output/speed.txt";
    std::string const ray_file = "../output/ray.txt";
    std::string const smoke_rho_file = "../output/smoke_rho.txt";
    std::string const atm_rho_file = "../output/atm_rho.txt";
    std::string const temp_file = "../output/temp.txt";
    std::ofstream altitude = std::ofstream(altitude_file.c_str());
    std::ofstream plumex = std::ofstream(plumex_file.c_str());
    std::ofstream speed = std::ofstream(speed_file.c_str());
    std::ofstream ray = std::ofstream(ray_file.c_str());
    std::ofstream smoke_rho = std::ofstream(smoke_rho_file.c_str());
    std::ofstream atm_rho = std::ofstream(atm_rho_file.c_str());
    std::ofstream temp = std::ofstream(temp_file.c_str());
    // Seed export
    std::string const seed_file = "../output/seed.txt";
    std::ofstream seed_ofstream = std::ofstream(seed_file.c_str());

    // General functions
    void setup_data(scene_structure& scene, gui_structure& gui);
    void setup_plume_params();

    void frame_draw(scene_structure& scene, gui_structure& gui);
    void display(scene_structure& scene);

    void display_smoke_layers(Plume* plume);
    void display_billboards(Plume* plume);
    void display_free_spheres(Plume* plume);
    void display_spheres_with_subspheres(Plume* plume);
    void display_subspheres(Plume* plume);
    void display_falling_spheres(Plume* plume);
    void display_falling_spheres_buffers(Plume* plume);

    void reset_simulation();
    void setup_terrain_preemptive();

    // Fill structures
    void fill_height_field(vcl::buffer<vcl::vec3>& position, vcl::buffer<vcl::vec3>& normal,
                           vcl::mesh_drawable terrain);
    // Smoke layer computation
    vcl::vec3 compute_wind_vector(float height);
    void calculate_avg_wind_dir();
    // Input
    void keyboard_input(scene_structure& scene, GLFWwindow* window, int key, int scancode, int action, int mods);

    // Init
    void set_gui(gui_structure& gui);
    void set_gui_playback(gui_structure& gui);
    void set_gui_profiler(gui_structure& gui);

    gui_parameters gui_param;
};


