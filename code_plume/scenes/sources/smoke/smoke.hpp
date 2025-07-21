#pragma once

#include "scenes/base/base.hpp"
#include "scenes/sources/smoke/smokeLayer.hpp"
#include "scenes/sources/smoke/terrain_structure.hpp"
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

struct wind_structure
{
    int intensity;
    float angle;
    vcl::vec3 wind_vector; // horizontal

    wind_structure() : intensity(0), angle(0), wind_vector(1,0,0) {}
    wind_structure(int intensity, int angle) : intensity(intensity), angle(angle * (3.14159 / 180))
    {
    }
    void recalc_wind_vector();
};

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
    bool debug_mode;
    float t_step;
    bool replay;
    size_t frame_replay;
    bool export_data;
    engine_state state;
    std::vector<int> deg_angle;
    bool all_angles;
    int gui_index;

    // Trackers
    float sim_time;
    float new_layer_delay;
    unsigned int total_layers_ejected;
    unsigned int nb_of_iterations;
    unsigned int last_ppe_layer_idx;
    float decal_progress = 1.f;
    float avg_wind_dir_degrees;
    std::vector<std::string> tooltip_names;
    std::vector<std::string> landmark_names;
    // Meshes
    vcl::mesh mesh_terrain;

    vcl::mesh_drawable generic_torus_mesh;
    vcl::mesh_drawable generic_sphere_mesh;
    vcl::mesh_drawable layer_mesh;
    vcl::mesh_drawable terrain;
    vcl::mesh_drawable skysphere;
    vcl::mesh_drawable terrain_display;
    vcl::mesh_drawable tooltip_display[4];
    vcl::mesh_drawable landmark_display[11];
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

    // Parameters : to be chosen by user
    double T_0; // initial temp
    double theta_0; // initial angle
    double U_0; // initial speed
    double n_0; // initial gas mass fraction
    double z_0; // initial altitude
    double r_0; // initial radius
    double rho_0; // initial density
    double air_incorporation_coeff;
    double stagnation_speed;

    unsigned int subspheres_number;
    unsigned int subsubspheres_number;

    std::vector<int> wind_altitudes;
    std::vector<wind_structure> winds;
    float linear_wind_base;
    bool is_wind;
    int selected;
    int wind_alt;

    // Parameters : constants
    float g;
    float max_lifetime;

    float tooltip_dist;
    float landmark_min_dist;
    float landmark_max_dist;

    int max_smoke;
    float transition_speed;
    float transition_delay;

    float max_altitude;
    float altitude_step;
    int altitude_size;

    int direction_tracker_step_size;
    float direction_tracker_step;

    // Data structures
    std::vector<smoke_layer> smoke_layers;
    std::vector<free_sphere_params> free_spheres;
    std::vector<subsphere_params> s2_spheres;
    std::vector<subsphere_params> s3_spheres;
    std::vector<free_sphere_params> stagnate_spheres;
    std::vector<free_sphere_params> falling_spheres;
    std::vector< std::vector<free_sphere_params> > falling_spheres_buffers;
    std::vector<float> sphere_lifetime;
    std::vector<float> transition_lifetime;

    terrain_structure terrain_struct;
    terrain_loader t_loader;
    direction_tracker direction_tracker;
    tooltip_loader tip_loader;
    landmark_loader mark_loader;

    // For replay feature
    std::vector< std::vector<smoke_layer> > smoke_layers_frames;
    std::vector< std::vector<free_sphere_params> > free_spheres_frames;
    std::vector< std::vector<subsphere_params> > s2_spheres_frames;
    std::vector< std::vector<free_sphere_params> > stagnate_spheres_frames;
    std::vector< std::vector<free_sphere_params> > falling_spheres_frames;
    std::vector< std::vector< std::vector<free_sphere_params> > > falling_spheres_buffers_frames;

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
    void setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void display(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void display_replay(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui, size_t frame);
    void reset_simulation();
    void setup_terrain_preemptive();

    // Smoke layer computation
    vcl::vec3 compute_wind_vector(float height);
    void calculate_avg_wind_dir();
    void add_smoke_layer(float v, float d, float r, vcl::vec3 position, bool secondary_plume);
    void edit_smoke_layer_properties(unsigned int i, float& d_mass);
    void apply_forces_to_smoke_layer(unsigned int i, float d_mass);
    void sedimentation(unsigned int i, float& d_mass);
    void pyroclastic_flow_computation_step(unsigned int i);
    void complete_plume_layer_properties_update(unsigned int i);
    void smoke_layer_update(unsigned int i);
    void check_smoke_position(unsigned int i);
    void remove_smoke_layers();

    // Pyroclastic flow : falling spheres
    float field_height_at(float x, float y);
    vcl::vec3 field_normal_at(float x, float y);
    void sphere_ground_collision(free_sphere_params& sphere, int idx, unsigned int frame_nb);
    void ground_falling_sphere_update(free_sphere_params& sphere, int idx, unsigned int frame_nb);
    void secondary_columns_creation();
    void falling_spheres_update(unsigned int frame_nb);

    // Free spheres
    void add_free_sphere(unsigned int i, float angle, float size_fac);
    void add_free_spheres_for_one_layer(unsigned int i);
    void subdivide_and_make_falling(unsigned int i);
    void update_free_spheres();

    // Stagnation
    void update_stagnation_spheres();

    // Export
    void update_subspheres_params();
    void export_spheres();

    // Fill structures
    void fill_height_field(vcl::buffer<vcl::vec3>& position, vcl::buffer<vcl::vec3>& normal,
                           vcl::mesh_drawable terrain);

    // Input
    void keyboard_input(scene_structure& scene, GLFWwindow* window, int key, int scancode, int action, int mods);

    // Init
    void set_gui(gui_structure& gui);
    void set_gui_playback(gui_structure& gui);
    void set_gui_profiler(gui_structure& gui);

    gui_parameters gui_param;
};


