#pragma once
#include "scenes/sources/smoke/smokeLayer.hpp"
#include "scenes/sources/smoke/terrain_structure.hpp"
#include "scenes/sources/smoke/wind_structure.hpp"
#include <vector>

struct Plume
{
    // User-defined parameters
    double T_0; // initial temp
    double theta_0; // initial angle
    double U_0; // initial speed
    double n_0; // initial gas mass fraction
    double z_0; // initial altitude
    double r_0; // initial radius
    double rho_0; // initial density
    double air_incorporation_coeff;
    double stagnation_speed;

    // Trackers
    float t_step;

    float new_layer_delay;
    unsigned int total_layers_ejected;
    unsigned int nb_of_iterations;
    unsigned int last_ppe_layer_idx;

    unsigned short free_sphere_id;
    unsigned short falling_sphere_id;

    unsigned int subspheres_number; // IU subsphere count
    unsigned int subsubspheres_number; // IU subsubsphere count

    // Constants
    float g;
    float min_lifetime;
    float max_lifetime;
    vcl::vec3 vent_position;

    // Data structures
    std::vector<smoke_layer> smoke_layers;
    std::vector<free_sphere_params> free_spheres;
    std::vector<subsphere_params> s2_spheres;
    std::vector<subsphere_params> s3_spheres;
    std::vector<free_sphere_params> stagnate_spheres;
    std::vector<free_sphere_params> falling_spheres;
    std::vector< std::vector<free_sphere_params>> falling_spheres_buffers;

    std::vector<float> sphere_lifetime;
    std::vector<float> transition_lifetime;

    Plume();
    void reset();
    void step();
    void update(unsigned int frame_count);

    // Smoke layer computation
    void add_smoke_layer(float v, float d, float r, vcl::vec3 position, bool secondary_plume);
    void edit_smoke_layer_properties(unsigned int i, float& d_mass);
    void apply_forces_to_smoke_layer(unsigned int i, float d_mass);
    void sedimentation(unsigned int i, float& d_mass);
    void pyroclastic_flow_computation_step(unsigned int i);
    void complete_plume_layer_properties_update(unsigned int i);
    void smoke_layer_update(unsigned int i);
    void remove_colliding_smoke();
    void remove_smoke_layers();

    float compute_gaussian_speed_in_layer(float v_z, float max_r, float r);
    float compute_atm_temperature(float height);
    float compute_atm_density(float height);

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
};