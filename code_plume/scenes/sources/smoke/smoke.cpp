#include "smoke.hpp"
#include "singleton/CameraManager.hpp"
#include "singleton/ShaderManager.hpp"

using namespace vcl;



// Counter used to save image on hard drive
int counter_image = 0;

std::chrono::system_clock::time_point a = std::chrono::system_clock::now();
std::chrono::system_clock::time_point b = std::chrono::system_clock::now();

//------------------------------------------------------------
//------------------------ TERRAIN ---------------------------
//------------------------------------------------------------

void scene_model::fill_height_field(vcl::buffer<vcl::vec3>& position, vcl::buffer<vcl::vec3>& normal,
                                    vcl::mesh_drawable terrain)
{
    // prepare field with parameters
    terrain_struct.min_xyz = -15000.0f;
    terrain_struct.max_xyz = 15000.0f;
    float interval_size = terrain_struct.max_xyz - terrain_struct.min_xyz;
    terrain_struct.cell_size = 200.f;
    terrain_struct.field_size = (size_t)(terrain_struct.max_xyz/terrain_struct.cell_size);
    terrain_struct.height_field.resize(terrain_struct.field_size, terrain_struct.field_size);
    terrain_struct.normal_field.resize(terrain_struct.field_size, terrain_struct.field_size);

    //transform like for mesh_drawable
    mat3 rotation = terrain.uniform.transform.rotation;
    vec3 translation = terrain.uniform.transform.translation;
    float scaling = terrain.uniform.transform.scaling;
    for (unsigned int i = 0; i<position.size(); i++)
    {
        position[i] = scaling * (rotation * position[i] + translation);
        normal[i] = rotation * normal[i];
    }
    terrain_struct.positions = position;
    terrain_struct.normals = normal;


    //fill height field for collisions
    for (unsigned int i = 0; i<position.size(); i++)
    {
        if (position[i].x < terrain_struct.max_xyz && position[i].x > terrain_struct.min_xyz
                && position[i].y < terrain_struct.max_xyz && position[i].y > terrain_struct.min_xyz)
        {
            int idx_x = (int)(terrain_struct.field_size*(position[i].x - terrain_struct.min_xyz)/interval_size);
            int idx_y = (int)(terrain_struct.field_size*(position[i].y - terrain_struct.min_xyz)/interval_size);
            if (idx_x == terrain_struct.field_size) idx_x = terrain_struct.field_size-1;
            if (idx_y == terrain_struct.field_size) idx_y = terrain_struct.field_size-1;
            terrain_struct.height_field(idx_x,idx_y) = position[i].z;
            terrain_struct.normal_field(idx_x,idx_y) = normal[i];

            //std::cout << "idx_x: " << idx_x << " idx_y: " << idx_y << " height: " << position[i].z << "\n";
        }
    }

}


//------------------------------------------------------------
//------------------------ PROCESS ---------------------------
//------------------------------------------------------------ */

void scene_model::frame_draw(scene_structure& scene, gui_structure& gui)
{
    // Maintain designated frequency of 5 Hz (200 ms per frame)
//    a = std::chrono::system_clock::now();
//    std::chrono::duration<double, std::milli> work_time = a - b;

//    if (work_time.count() < 16.0)
//    {
//        std::chrono::duration<double, std::milli> delta_ms(16.0 - work_time.count());
//        auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
//        std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));
//    }

//    b = std::chrono::system_clock::now();
//    std::chrono::duration<double, std::milli> sleep_time = b - a;

    setup_terrain_preemptive();

    dt = timer.update();
    set_gui(gui);
    set_gui_playback(gui);
    set_gui_profiler(gui);
    t_loader.show_gui(&gui.enabled["Terrain"]);
    direction_tracker.show_gui(&gui.enabled["Direction Tracker"]);

    terrain_display.texture_id = t_loader.current_tex_id;
    terrain_display.norm_tex_id = t_loader.current_norm_id;
    //std::cout << terrain_display.texture_id << std::endl;
    //std::cout << t_loader.current_tex_id << std::endl;

    //TODO: fix error with smoke layers suddenly exploding
    // Force constant time step
    t_step = dt<=1e-6f? 0.0f : timer.scale*0.002f; //0.0003f
    new_layer_delay += t_step;

    for (int i = 0; i < plumes.size(); i++)
    {
        plumes[i]->t_step = t_step;
        plumes[i]->new_layer_delay = new_layer_delay;
    }
    
    if (!replay)
    {
        sim_time += t_step;
        for (int i = 0; i < plumes.size(); i++)
        {
            //plumes[i].remove_colliding_smoke();
            plumes[i]->remove_smoke_layers();
        }

        for (int i = 0; i < transition_lifetime.size(); i++)
        {
            transition_lifetime[i] += t_step;
        }

        for (unsigned int nb_steps_per_frame = 0; nb_steps_per_frame<10; nb_steps_per_frame++)
        {
            for (int i = 0; i < plumes.size(); i++)
            {
                plumes[i]->step();
                plumes[i]->update(frame_count);
            }
            frame_count++;
        }
    }

    if (replay)
    {

        //display_replay(shaders, scene, gui, frame_replay);
        //std::cout << frame_replay << std::endl;
        //frame_replay++;
        //frame_replay = frame_replay % smoke_layers_frames.size();

    }
    else display(scene);
}


//------------------------------------------------------------
//------------------------- ALGO -----------------------------
//------------------------------------------------------------ */

vcl::vec3 scene_model::compute_wind_vector(float height)
{
    // find altitude interval
    unsigned int low_altitude_idx = 0;
    for (unsigned int i = 0; i<wind_altitudes.size(); i++)
    {
        if (wind_altitudes[i] < height)
        {
            low_altitude_idx = i;
        }
    }

    // compute wind vec by interpolating
    if (low_altitude_idx == wind_altitudes.size()-1)
    {
        return winds[low_altitude_idx].wind_vector;
    }
    else
    {
        float low_height = (float)wind_altitudes[low_altitude_idx];
        float high_height = (float)wind_altitudes[low_altitude_idx+1];
        float lambda = (height-low_height)/(high_height-low_height);
        vec3 interpo_wind = winds[low_altitude_idx].wind_vector + lambda * (winds[low_altitude_idx+1].wind_vector - winds[low_altitude_idx].wind_vector);
        return interpo_wind;
    }
}

// TODO: transfer logic to direction tracker using WindManager
void scene_model::calculate_avg_wind_dir()
{
    vcl::vec3 winds_vec = { 0,0,0 };
    for (int i = 0; i < winds.size(); i++)
    {
        winds_vec += winds[i].wind_vector;
    }

    float winds_squared_x = winds_vec.x * winds_vec.x;
    float winds_squared_y = winds_vec.y * winds_vec.y;
    float winds_squared_z = winds_vec.z * winds_vec.z;

    float mag = sqrt(winds_squared_x + winds_squared_y + winds_squared_z);
    this->avg_wind_direction = vcl::vec3(winds_vec.x, winds_vec.y, winds_vec.z) / mag;
    direction_tracker.set_wind_direction(this->avg_wind_direction);
}

//------------------------------------------------------------
//------------------------- SETUP ----------------------------
//------------------------------------------------------------ */

void scene_model::setup_data(scene_structure& scene, gui_structure& gui)
{
    // begin with timer stopped
    timer.stop();
    replay = false;
    frame_count = 0;
    sim_time = 0;
    export_data = false;
    state = engine_state::stopped;
    all_angles = false;
    srand(time(0));

    t_loader.mesh_shader = ShaderManager::getInstance()->getShader("mesh");
    t_loader.load_all_textures();

    gui_param.display_smoke_layers = false;
    gui_param.display_free_spheres = false;
    gui_param.display_subspheres = false;
    gui_param.display_spheres_with_subspheres = false;
    gui_param.display_billboards = true;
    gui_param.display_tooltips = true;
    //decal = t_loader.texture_id[1];
    debug_mode = true;
    float seed = time(0);
    srand(seed);
    if (seed_ofstream)
    {
        seed_ofstream.precision(10);
        seed_ofstream << seed;
    }

    gui.show_frame_camera = false; std::cout << "replay becomes false 0" << std::endl;
    gui.enabled["Simulator Input"] = true;
    gui.enabled["Direction Tracker"] = true;
    gui.enabled["Playback"] = true;
    gui.enabled["Profiler"] = true;
    gui.enabled["Terrain"] = true;

    setup_plume_params();

    // Meshes setup
    layer_mesh = mesh_drawable( mesh_primitive_cylinder(0.1f, {0,0,0}, {0,0,0.01}));
    layer_mesh.shader = ShaderManager::getInstance()->getShader("mesh");
    layer_mesh.uniform.color = {0,0.5,1};

    mesh cyl = vcl::mesh_primitive_cylinder(2.5f, {0,0,1.5}, {0,0,-1.5}, 30, 30);
    mesh d1 = vcl::mesh_primitive_disc(2.5f, {0,0,1.5});
    mesh d2 = vcl::mesh_primitive_disc(2.5f, {0,0,-1.5});
    mesh t = cyl;t.push_back(d1); t.push_back(d2);

    generic_sphere_mesh = vcl::mesh_primitive_sphere();
    generic_sphere_mesh.texture_id = scene.texture_white;
    //generic_torus_mesh = vcl::mesh_primitive_torus(1.,1.,{0,0,0}, {0,0,-1});
    generic_torus_mesh = t;
    generic_torus_mesh.uniform.color = {1,0.5,0};
    generic_torus_mesh.shader = ShaderManager::getInstance()->getShader("mesh");
    generic_torus_mesh.texture_id = scene.texture_white;
    generic_torus_mesh.uniform.color_alpha = 0.6f;
    //texture_smoke_id = create_texture_gpu( image_load_png("../scenes/sources/smoke/images/texture_panache.png") );
    pauseIcon = create_texture_gpu(image_load_png("../scenes/sources/smoke/images/pause_icon.png"));
    playIcon = create_texture_gpu(image_load_png("../scenes/sources/smoke/images/play_icon.png"));
    resetIcon = create_texture_gpu(image_load_png("../scenes/sources/smoke/images/undo_icon.png"));

    sphere = mesh_drawable( mesh_primitive_sphere(0.1f));
    sphere.shader = ShaderManager::getInstance()->getShader("mesh");
    sphere.uniform.color = {0,0.5,1};
    sphere.texture_id = scene.texture_white;

    subspheres = vcl::mesh_primitive_sphere(1.0, {0,0,0}, 10 ,20);
    subspheres.texture_id = create_texture_gpu(image_load_png("../scenes/sources/smoke/smoke_tex/IMG_2765.png"));
    subspheres.uniform.color = {0.6,0.5,0.5};
    subspheres.uniform.shading.diffuse = 0.8f;
    subspheres.uniform.shading.specular = 0.0f;

    smoke_texture = create_texture_gpu(image_load_png("../scenes/sources/smoke/smoke_tex/smoke-tex-0.png"));
    quad = mesh_drawable(mesh_primitive_quad({-1,-1,0},{1,-1,0},{1,1,0},{-1,1,0}));
    quad.uniform.shading.ambiant = 1.0;
    quad.uniform.shading.diffuse = 0.0;
    quad.uniform.shading.specular = 0.0;

    max_smoke = 20;
    transition_speed = 5.0f;
    transition_delay = 0.2f;
    for (int i = 0; i < max_smoke; i++)
        transition_lifetime.push_back(transition_delay * i);

    //sky mesh setup
    sphere = mesh_drawable(mesh_primitive_sphere(100.0f));
    sphere.shader = ShaderManager::getInstance()->getShader("sky_mesh");
    sphere.uniform.color = { 1,1,1 };
    sphere.texture_id = scene.texture_white;

    mesh sky = mesh_load_file_obj("../scenes/sources/smoke/Skydome/Taal_Skydome.obj");
    skysphere = mesh_drawable(sky);
    //skysphere = mesh_drawable(mesh_primitive_sphere(100.0f));
    skysphere.texture_id = create_texture_gpu(image_load_png("../scenes/sources/smoke/Skydome/Skysphere_Tex.png"));
    skysphere.uniform.color = { 1, 1, 1 };
    skysphere.uniform.shading.specular = 100.0f;
    skysphere.uniform.shading.ambiant = 1.0f;
    skysphere.uniform.shading.diffuse = 1.0f;
    skysphere.uniform.transform.rotation = rotation_from_axis_angle_mat3({ 1.0f,0,0 }, 3.14f / 2.0f);
    skysphere.uniform.transform.scaling = 1.0f;
    skysphere.uniform.transform.translation = { 0,0,0 };

    ////skybox setup
    //std::vector<image_raw> skybox_tex_raw;
    //for (int i = 0; i < 6; i++)
    //{
    //    std::string path = "../scenes/sources/smoke/skybox_tex/skybox-partial-";
    //    path += std::to_string(i) + ".png";
    //    skybox_tex_raw.push_back(image_load_png(path));
    //}
    //skybox_tex = create_texture_cube_map_gpu(skybox_tex_raw);
    //skybox = skybox_drawable(vcl::skybox(), shaders["skybox"], skybox_tex);

    //auto circle = vcl::curve_primitve_circle(30, 1.0, {0,0,0}, {0,0,1});
    //sphere_circle = curve_drawable(circle);
    //sphere_circle.shader = shaders["curve"];
    //sphere_circle.uniform.color = {1,0,0};

    //sampling subpheres
    {
        int N = 60;
        for (int k = 0; k < N; ++k)
        {
            //uniform sampling on sphere
            float theta = 2*3.14f*vcl::rand_interval();
            float phi   = std::acos(1-2.0f*vcl::rand_interval());


            float x = std::sin(phi)*std::cos(theta);
            float y = std::sin(phi)*std::sin(theta);
            float z = std::cos(phi);

            vec3 p = {x,y,z};
            bool add = true;
            for (int k2 = 0; add==true && k2 < k; ++k2)
                if(norm(p-samples_subspheres[k2])<0.18f)
                    add=false;
            samples_subspheres.push_back({x,y,z});
        }
    } 

    {
        mesh m0 = vcl::mesh_primitive_sphere(1.0, {0,0,0}, 5 ,5);
        mesh m1 = vcl::mesh_primitive_sphere(1.0, {0,0,0}, 8 ,8);
        mesh m2 = vcl::mesh_primitive_sphere(1.0, {0,0,0}, 10 , 10);

        int N = 60;
        for (int k = 0; k < N; ++k)
        {
            //uniform sampling on sphere
            float theta = 2*3.14f*vcl::rand_interval();
            float phi   = std::acos(1-2.0f*vcl::rand_interval());
            float r = vcl::rand_interval(0.8f,1.0f);

            float x = r*std::sin(phi)*std::cos(theta);
            float y = r*std::sin(phi)*std::sin(theta);
            float z = r*std::cos(phi);

            vec3 p = {x,y,z};
            bool add = true;
            for (int k2 = 0; add==true && k2 < k; ++k2)
                if(norm(p-samples_subspheres[k2])<0.18f)
                    add=false;
            samples_subspheres.push_back({x,y,z});
        }

        mesh m;
        m.push_back(m0);
        for (int sub = 0; sub < samples_subspheres.size(); ++sub) {
            mesh temp = m1;
            float r = vcl::rand_interval(0.18f,0.2f);

            // subspheres
            for (int k = 0; k < temp.position.size(); ++k)
            {
                vec3 p = r*temp.position[k] + samples_subspheres[sub];

                vec3 n0 = temp.normal[k];
                vec3 n1 = normalize(p);

                float d = norm(p);
                float alpha = 0.0;
                if(d>1.0f && d<1.2f)
                    alpha = (d-1.0f)/0.2f;
                if(d>1.2f)
                    alpha = 1.0f;

                vec3 n = (1-alpha)*n1 + alpha*n0; // hack normals
                temp.normal[k] = n;
                temp.position[k] = p;
            }

            m.push_back(temp);
        }

        subspheres_display = mesh_drawable(m) ;

        subspheres_display.texture_id = scene.texture_white;
        subspheres_display.uniform.color = {0.6,0.6,0.55};
        subspheres_display.uniform.shading.ambiant = 0.7f;
        subspheres_display.uniform.shading.diffuse = 0.3f;
        subspheres_display.uniform.shading.specular = 0.0f;
    }
    
    tip_loader.setup_tooltips();
    mark_loader.setup_landmarks();

    //load terrain
    t_loader.load_terrain("Taal-Spherical-2_0.obj", "Taal_Texture_2023.png");

    terrain_display = t_loader.terrain;
    terrain_display.uniform.transform.scaling = .25f;
    //terrain_display.texture_id = create_texture_gpu(image_load_png("../scenes/sources/smoke/terrains/Taal_Texture_BaseColor_2016.png"));
    //terrain_display.norm_tex_id = add_normal_map(image_load_png("../scenes/sources/smoke/textures/Taal_Texture_normal_2024.png"));
    terrain_display.uniform.color = { 1,1,1 };

    // Params setup
    is_wind = false;
    linear_wind_base = 15.;
    max_altitude = 10000;
    altitude_step = 2000;
    altitude_size = int(max_altitude / altitude_step) + 1;
    for(unsigned int i = 0; i < altitude_size; i++)
    {
        wind_altitudes.push_back(i* altitude_step);
        winds.push_back(wind_structure(0,0));
        this->deg_angle.push_back(0);
    }

    is_wind = false;
    linear_wind_base = 15.;
    selected = 0;
    wind_alt = 0;

    // Direction tracker setup
    direction_tracker_step = 1000.0f;
    direction_tracker_step_size = 20;
    direction_tracker.initialize(max_altitude, direction_tracker_step_size);
    direction_tracker.load_data("../scenes/sources/smoke/taal_danger_zones.csv");
    calculate_avg_wind_dir();

}

void scene_model::setup_plume_params()
{

    // Vent positions setup
    vent_index = 0;
    vent_positions.push_back(vec3(2500, 0, 0));
    vent_positions.push_back(vec3(5850, 5950, 0));
    vent_positions.push_back(vec3(-2000, -6000, 0));
    vent_positions.push_back(vec3(-3000, 5800, 0));
    
    plumes.push_back(new Plume(vent_positions[0]));
}


void scene_model::display(scene_structure& scene)
{
    camera_scene* camera = CameraManager::getInstance()->getCamera();
    float ratio = 100;

    if (camera->sky_enabled)
        skysphere.draw_sky(*camera, ShaderManager::getInstance()->getShader("sky_mesh"), skysphere.texture_id);
        //draw_sky(sky_sphere, *camera, shaders["sky_mesh"], scene.texture_white);

    if (terrain_display.data.number_triangles > 0)
    {
        //draw(terrain_display, *camera, ShaderManager::getInstance()->getShader("mesh"), true);
        terrain_display.draw_mix(*camera, ShaderManager::getInstance()->getShader("mesh_mix"), terrain_display.texture_id, terrain_display.norm_tex_id, decal, 1);
    }
    //draw(terrain, *camera, shaders["wireframe"]);

    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i = 0; i < plumes.size(); i++)
    {
        // Display torus
        if (gui_param.display_smoke_layers) display_smoke_layers(plumes[i]);
        // billboards
        if (gui_param.display_billboards) display_billboards(plumes[i]);
        // free + stagnation spheres display
        if (gui_param.display_free_spheres) display_free_spheres(plumes[i]);
        // spheres+subspheres display (lighter)
        if (gui_param.display_spheres_with_subspheres) display_spheres_with_subspheres(plumes[i]);
        // subspheres display
        if (gui_param.display_subspheres) display_subspheres(plumes[i]);
        // falling spheres display
        if (gui_param.display_free_spheres) display_falling_spheres(plumes[i]);
        // buffer falling spheres display
        if (gui_param.display_free_spheres) display_falling_spheres_buffers(plumes[i]);
    }
    
    if (gui_param.display_tooltips == true  && camera->mode != view_mode::orbital) tip_loader.draw();

    mark_loader.draw();
}

#pragma region Unique Display

void scene_model::display_smoke_layers(Plume* plume)
{
    camera_scene* camera = CameraManager::getInstance()->getCamera();
    float ratio = 100;

    for (unsigned int i = 0; i < plume->smoke_layers.size(); i++)
    {
        smoke_layer lay = plume->smoke_layers[i];

        generic_torus_mesh.uniform.transform.scaling = lay.r / ratio;
        generic_torus_mesh.uniform.transform.translation = vec3(lay.center.x / ratio - 25, lay.center.y / ratio, lay.center.z / ratio - 2);
        generic_torus_mesh.uniform.transform.rotation = rotation_from_axis_angle_mat3(lay.theta_axis, lay.theta - 3.14 / 2.0);
        generic_torus_mesh.shader = ShaderManager::getInstance()->getShader("mesh");
        generic_torus_mesh.draw(*camera);
    }
}

void scene_model::display_billboards(Plume* plume)
{
    camera_scene* camera = CameraManager::getInstance()->getCamera();
    float ratio = 100;

    glDepthMask(false);

    // transition smoke
    for (int j = 0; j < transition_lifetime.size(); j++)
    {
        float animation = fmax(0, sinf(transition_speed * transition_lifetime[j]));
        float new_scaling = animation == 0 ? 4 : 2.0f + (animation * 2.0f);
        float offset = terrain_display.uniform.transform.translation.z;
        vec3 new_translation = vec3(0, 0, offset + (animation * (fabs(offset) - 2)));
        float var = vcl::perlin(j, 2);

        quad.uniform.transform.rotation = rotation_from_axis_angle_mat3(camera->orientation.col(2), transition_speed * transition_lifetime[j] * var) * camera->orientation;
        quad.uniform.transform.translation = new_translation;
        quad.uniform.transform.scaling = new_scaling * 1.3;
        quad.uniform.color = { 0.3f,0.3f,0.3f };
        quad.uniform.color_alpha = (0.8 + 0.3f * (2 * var - 1.0f)) * fmax(0.2f, animation);
        quad.shader = ShaderManager::getInstance()->getShader("mesh");
        quad.texture_id = smoke_texture;

        quad.draw(*camera);
    }


    for (unsigned int j = 0; j < plume->free_spheres.size(); j++)
    {
        mat3 const R = rotation_from_axis_angle_mat3(plume->free_spheres[j].rotation_axis, plume->free_spheres[j].current_angle);
        float new_scaling = plume->free_spheres[j].r / ratio;
        //if (j==0) std::cout << new_scaling << std::endl;
        vec3 new_translation = vec3(plume->free_spheres[j].center.x / ratio - 25, plume->free_spheres[j].center.y / ratio, plume->free_spheres[j].center.z / ratio - 2);
        generic_sphere_mesh.uniform.transform.translation = new_translation;
        generic_sphere_mesh.uniform.transform.scaling = new_scaling;
        generic_sphere_mesh.uniform.transform.rotation = R;
        generic_sphere_mesh.uniform.color = { 1,1,1 };
        generic_sphere_mesh.shader = ShaderManager::getInstance()->getShader("mesh");

        float var = vcl::perlin(plume->free_spheres[j].id, 2);

        //quad.uniform.transform.rotation = rotation_from_axis_angle_mat3(camera->orientation.col(2), free_spheres[j].current_angle * dot(free_spheres[j].rotation_axis, camera->orientation.col(2)) * 1.5f *(1+0.3*var) + 2.2145*j*j) * camera->orientation;
        quad.uniform.transform.rotation = rotation_from_axis_angle_mat3(camera->orientation.col(2), plume->free_spheres[j].id * var) * camera->orientation;
        quad.uniform.transform.translation = new_translation;
        quad.uniform.transform.scaling = new_scaling * 1.3;
        quad.uniform.color_alpha = 0.8 + 0.3f * (2 * var - 1.0f);

        float l = (plume->free_spheres[j].lifetime / 120) + 0.3f;
        if (l > 1) l = 1;
        quad.uniform.color = { l,l,l };

        float end_fade = 1.0f;
        if (plume->free_spheres[j].lifetime >= plume->min_lifetime)
            end_fade -= (plume->free_spheres[j].lifetime - plume->min_lifetime) / (plume->max_lifetime - plume->min_lifetime);
        quad.uniform.color_alpha *= end_fade;
        quad.shader = ShaderManager::getInstance()->getShader("mesh");

        quad.draw(*camera);
    }
    glDepthMask(true);
}

void scene_model::display_free_spheres(Plume* plume)
{
    camera_scene* camera = CameraManager::getInstance()->getCamera();
    float ratio = 100;

    for (unsigned int j = 0; j < plume->free_spheres.size(); j++)
    {
        //if (!free_spheres[j].falling)
        mat3 const R = rotation_from_axis_angle_mat3(plume->free_spheres[j].rotation_axis, plume->free_spheres[j].current_angle);
        float r = plume->free_spheres[j].r / ratio;
        vec3 t = vec3(plume->free_spheres[j].center.x / ratio - 25, plume->free_spheres[j].center.y / ratio, plume->free_spheres[j].center.z / ratio - 2);
        float rho = plume->free_spheres[j].rho;
        float disp_rho = 1. - rho;
        if (disp_rho < 0) disp_rho = 0.;
        disp_rho = 1.;

        generic_sphere_mesh.uniform.transform.translation = t;
        generic_sphere_mesh.uniform.transform.scaling = r;
        generic_sphere_mesh.uniform.transform.rotation = R;
        generic_sphere_mesh.uniform.color = { disp_rho,disp_rho,disp_rho };
        generic_sphere_mesh.shader = ShaderManager::getInstance()->getShader("mesh");
        generic_sphere_mesh.draw(*camera);
    }
}

void scene_model::display_spheres_with_subspheres(Plume* plume)
{
    camera_scene* camera = CameraManager::getInstance()->getCamera();
    float ratio = 100;
    for (unsigned int j = 0; j < plume->free_spheres.size(); j++)
    {
        //if (!free_spheres[j].falling)
        mat3 const R = rotation_from_axis_angle_mat3(plume->free_spheres[j].rotation_axis, plume->free_spheres[j].current_angle);
        float r = plume->free_spheres[j].r / ratio;
        vec3 t = vec3(plume->free_spheres[j].center.x / ratio - 25, plume->free_spheres[j].center.y / ratio, plume->free_spheres[j].center.z / ratio - 2);
        float rho = plume->free_spheres[j].rho;
        float disp_rho = 1. - rho;
        if (disp_rho < 0) disp_rho = 0.;
        disp_rho = 1.;

        subspheres_display.uniform.transform.translation = t;
        subspheres_display.uniform.transform.scaling = r;
        subspheres_display.uniform.transform.rotation = R;
        subspheres_display.uniform.color = { disp_rho,disp_rho,disp_rho };
        subspheres_display.shader = ShaderManager::getInstance()->getShader("mesh");

        subspheres_display.draw(*camera);
    }
}

void scene_model::display_subspheres(Plume* plume)
{
    camera_scene* camera = CameraManager::getInstance()->getCamera();
    float ratio = 100;
    for (unsigned int j = 0; j < plume->s2_spheres.size(); j++)
    {
        if (!plume->free_spheres[plume->s2_spheres[j].parent_id].falling)
        {
            float r = plume->s2_spheres[j].r / ratio;
            vec3 t = plume->s2_spheres[j].center / ratio;
            float rho = plume->free_spheres[plume->s2_spheres[j].parent_id].rho;
            float disp_rho = 1. - rho;
            if (disp_rho < 0) disp_rho = 0.;
            disp_rho = 1.;

            generic_sphere_mesh.uniform.transform.translation = t;
            generic_sphere_mesh.uniform.transform.scaling = r;
            generic_sphere_mesh.uniform.color = { disp_rho,disp_rho,disp_rho };
            generic_sphere_mesh.shader = ShaderManager::getInstance()->getShader("mesh");
            generic_sphere_mesh.draw(*camera);
        }
    }
}

void scene_model::display_falling_spheres(Plume* plume)
{
    camera_scene* camera = CameraManager::getInstance()->getCamera();
    float ratio = 100;

    for (unsigned int j = 0; j < plume->falling_spheres.size(); j++)
    {
        float new_scaling = plume->falling_spheres[j].r / ratio;
        vec3 new_translation = { plume->falling_spheres[j].center.x / ratio, plume->falling_spheres[j].center.y / ratio, plume->falling_spheres[j].center.z / ratio - 2 };
        generic_sphere_mesh.uniform.transform.translation = new_translation;
        generic_sphere_mesh.uniform.transform.scaling = new_scaling;
        generic_sphere_mesh.uniform.transform.rotation = mat3::identity();
        if (plume->falling_spheres[j].falling_under_atm_rho) generic_sphere_mesh.uniform.color = { 1,0,0 };
        else generic_sphere_mesh.uniform.color = { 1,1,1 };
        generic_sphere_mesh.shader = ShaderManager::getInstance()->getShader("mesh");
        generic_sphere_mesh.draw(*camera);
    }
}

void scene_model::display_falling_spheres_buffers(Plume* plume)
{
    camera_scene* camera = CameraManager::getInstance()->getCamera();
    float ratio = 100;

    for (unsigned int k = 0; k < plume->falling_spheres_buffers.size(); k++)
    {
        for (unsigned int j = 0; j < plume->falling_spheres_buffers[k].size(); j++)
        {
            float new_scaling = plume->falling_spheres_buffers[k][j].r / ratio;
            vec3 new_translation = { plume->falling_spheres_buffers[k][j].center.x / ratio, plume->falling_spheres_buffers[k][j].center.y / ratio, plume->falling_spheres_buffers[k][j].center.z / ratio + 5 };
            generic_sphere_mesh.uniform.transform.translation = new_translation;
            generic_sphere_mesh.uniform.transform.scaling = new_scaling;
            generic_sphere_mesh.uniform.transform.rotation = mat3::identity();
            if (plume->falling_spheres_buffers[k][j].falling_under_atm_rho) generic_sphere_mesh.uniform.color = { 1,0,0 };
            else generic_sphere_mesh.uniform.color = { 1,1,1 };
            generic_sphere_mesh.shader = ShaderManager::getInstance()->getShader("mesh");
            generic_sphere_mesh.draw(*camera);
        }
    }
}

#pragma endregion

void scene_model::reset_simulation()
{
    timer.stop();
    direction_tracker.reset_plume_positions();
    frame_count = 0;
    sim_time = 0;

    for (int i = 0; i < plumes.size(); i++)
        plumes[i]->reset();

    transition_lifetime.clear();
    for (int i = 0; i < max_smoke; i++)
        transition_lifetime.push_back(transition_delay * i);

    //smoke_layers_frames.clear();
    //free_spheres_frames.clear();
    //s2_spheres_frames.clear();
    //stagnate_spheres_frames.clear();
    //falling_spheres_frames.clear();
    //falling_spheres_buffers_frames.clear();

    frame_replay = 0;
    replay = false;
    export_data = false;
}

void scene_model::setup_terrain_preemptive()
{
    if (t_loader.new_terrain_loaded)
    {
        t_loader.new_terrain_loaded = false;

        terrain_display = t_loader.terrain;
        terrain_display.uniform.transform.scaling = .25f;
        terrain_display.uniform.shading.ambiant = 1.0f;
        terrain_display.uniform.color = { 1,1,1 };

        //terrain_display.norm_tex_id = add_normal_map(image_load_png("../scenes/sources/smoke/textures/Taal_Texture_normal_2024.png"));


        std::cout << "Pre-emptive terrain setup triggered" << "\n";
    }
}

// TODO: Finally move GUI stuff into their respective screens
void scene_model::set_gui(gui_structure& gui)
{
    Plume* plume = plumes[0];
    ImGui::Begin("Simulator Input", &gui.enabled["Simulator Input"], ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 5);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1, 1, 1, 0.1f));
    ImGui::PushItemWidth(200);

    const float indent_width = 5;
    const float child_width = 380;
    
    // Can set the speed of the animation
    float scale_min = 0.05f;
    float scale_max = 5.0f;
    ImGui::SliderScalar("Time scale", ImGuiDataType_Float, &timer.scale, &scale_min, &scale_max, "%.2f s");

    // Parameters
    unsigned int spheres_min = 0, spheres_max = 500;
    ImGui::SliderScalar("Number of subspheres", ImGuiDataType_S32, &plume->subspheres_number, &spheres_min, &spheres_max);
    ImGui::SliderScalar("Number of subsubspheres", ImGuiDataType_S32, &plume->subsubspheres_number, &spheres_min, &spheres_max);
    ImGui::PopItemWidth();

    if (ImGui::CollapsingHeader("Display Settings", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::BeginChild("Display", ImVec2(child_width, ImGui::GetItemsLineHeightWithSpacing() * 5.25f));
        ImGui::Spacing();
        ImGui::Indent(indent_width);

        ImGui::Checkbox("Display billboards", &gui_param.display_billboards);
        ImGui::Checkbox("Display torus layers", &gui_param.display_smoke_layers);
        ImGui::Checkbox("Display free spheres", &gui_param.display_free_spheres);
        //ImGui::Checkbox("Display subspheres", &gui_param.display_subspheres);
        ImGui::Checkbox("Display spheres with subspheres", &gui_param.display_spheres_with_subspheres);
        ImGui::Checkbox("Display Tooltips", &gui_param.display_tooltips);
        ImGui::Unindent();
        ImGui::EndChild();
    }

    // Coeffs

    //float air_inc_min = 0.5, air_inc_max = 10.;
    //ImGui::SliderScalar("Air incorporation coefficient", ImGuiDataType_Float, &air_incorporation_coeff, &air_inc_min, &air_inc_max, "%.2f");

    // Initial conditions
    if (ImGui::CollapsingHeader("Eruption Parameters", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::BeginChild("Parameters", ImVec2(child_width, ImGui::GetItemsLineHeightWithSpacing() * 4.25f));
        ImGui::Spacing();
        ImGui::Indent(indent_width);
        ImGui::PushItemWidth(200);

        float initial_speed_min = 0., initial_speed_max = 200.;
        ImGui::SliderScalar("Initial plume speed", ImGuiDataType_Float, &plume->U_0, &initial_speed_min, &initial_speed_max, "%.2f m/s");
        float initial_density_min = 150., initial_density_max = 250.;
        ImGui::SliderScalar("Initial plume density", ImGuiDataType_Float, &plume->rho_0, &initial_density_min, &initial_density_max, "%.2f kg/m3");
        float vent_ray_min = 50., vent_ray_max = 200.;
        ImGui::SliderScalar("Vent radius", ImGuiDataType_Float, &plume->r_0, &vent_ray_min, &vent_ray_max, "%.2f m");
        float vent_altitude_min = 0., vent_altitude_max = 8000.;
        ImGui::SliderScalar("Vent altitude", ImGuiDataType_Float, &plume->z_0, &vent_altitude_min, &vent_altitude_max, "%.2f m");

        ImGui::PopItemWidth();
        ImGui::Unindent();
        ImGui::EndChild();

    }
    // Wind presets
    if (ImGui::CollapsingHeader("Wind Settings", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::BeginChild("Wind", ImVec2(child_width, ImGui::GetItemsLineHeightWithSpacing() * 13.5f));
        ImGui::Spacing();
        ImGui::Indent(indent_width);
        ImGui::PushItemWidth(200);

        const int wind_size = 6;
        bool altitude_selected = false;

        int alt_min = 0, alt_max = max_altitude;
        int wind_min = 0, wind_max = 200;
        int angle_min = 0, angle_max = 360;

        const float indent_w = 29;
        const float slider_width = 25;
        const float plot_width = 330;
        const float plot_height = 100;

        float intensity[wind_size] = {};
        float angle[wind_size] = {};

        for (int i = 0; i < wind_size; i++)
        {
            intensity[i] = winds[i].intensity;
            angle[i] = this->deg_angle[i];
        }

        const int x_offset = ImGui::GetCursorScreenPos().x;
        const int plot_start = ImGui::GetCursorScreenPos().y;

        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        const float plot_grid_offset = x_offset + slider_width + 11;
        for (int i = 0; i < 3; i++)
        {
            for (int j = 1; j < wind_size - 1; j++)
            {
                float plot_division = (float)j / (wind_size - 1);
                float plot_grid = (plot_division * (plot_width - 8)) + plot_grid_offset;

                float total_height = plot_start + plot_height * i + (i * 3);
                ImVec2 start = ImVec2(plot_grid, total_height);
                ImVec2 end;

                if (i == 2) end = ImVec2(plot_grid, total_height + 22);
                else end = ImVec2(plot_grid, total_height + plot_height);

                draw_list->AddLine(start, end, IM_COL32(255, 255, 255, 100), 1.0f);
            }
        }

        if (ImGui::VSliderScalar("##Intensity Slider", ImVec2(slider_width, plot_height), ImGuiDataType_S32, &winds[selected].intensity, &wind_min, &wind_max))
        {
            winds[selected] = wind_structure(winds[selected].intensity, this->deg_angle[selected]);
            winds[selected].recalc_wind_vector();
            calculate_avg_wind_dir();
        }

        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
        ImGui::PlotLines("##Wind Intensity", intensity, wind_size, 0, "Wind Intensity (m/s)", wind_min, wind_max, ImVec2(plot_width, plot_height));
        ImGui::PopStyleColor();

        if (ImGui::VSliderScalar("##Angle Slider", ImVec2(slider_width, plot_height), ImGuiDataType_S32, &this->deg_angle[selected], &angle_min, &angle_max))
        {
            if (all_angles)
            {
                for (int i = 0; i < wind_size; i++)
                {
                    this->deg_angle[i] = this->deg_angle[selected];
                    winds[i] = wind_structure(winds[i].intensity, this->deg_angle[i]);
                    winds[i].recalc_wind_vector();
                }
            }
            else
            {
                winds[selected] = wind_structure(winds[selected].intensity, this->deg_angle[selected]);
                winds[selected].recalc_wind_vector();
            }
            calculate_avg_wind_dir();
        }
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.1f, 0.8f, 0.1f, 1.0f));
        ImGui::PlotLines("##Wind Angle", angle, wind_size, 0, "Wind Angle (degrees)", angle_min, angle_max, ImVec2(plot_width, plot_height));
        ImGui::PopStyleColor();

        float plot_div = (float)selected / (wind_size - 1);
        float plot_x = (plot_div * (plot_width - 8)) + plot_grid_offset;

        ImVec2 start = ImVec2(plot_x, plot_start);
        ImVec2 end = ImVec2(plot_x, plot_start + (plot_height * 2) + 3);
        draw_list->AddLine(start, end, IM_COL32(240, 220, 40, 255), 3.0f);

        ImGui::Indent(indent_w);
        ImGui::PushItemWidth(plot_width + 7);
        if (ImGui::SliderScalar("##Altitude", ImGuiDataType_S32, &wind_alt, &alt_min, &alt_max, "%d meters in altitude"))
        {
            for (int i = 0; i < wind_size && !altitude_selected; i++)
            {
                if (wind_altitudes[i] == wind_alt)
                {
                    altitude_selected = true;
                    selected = i;
                }
            }

            if (!altitude_selected)
            {
                selected = clamp(((float)wind_alt / altitude_step) + 0.5f, 0, wind_size - 1);
                wind_alt = wind_altitudes[selected];
            }
        }
        ImGui::PopItemWidth();
        ImGui::Unindent(indent_w);
        ImGui::Spacing();

        if (ImGui::Button("No wind"))
        {
            is_wind = false;
            for (unsigned int i = 0; i < winds.size(); i++)
            {
                this->deg_angle[i] = angle_min;
                winds[i] = wind_structure(wind_min, this->deg_angle[i]);
                winds[i].recalc_wind_vector();
            }
            calculate_avg_wind_dir();
        }

        ImGui::SameLine();
        if (ImGui::Button("Linear Wind"))
        {
            is_wind = true;
            for (unsigned int i = 0; i < winds.size(); i++)
            {
                winds[i].intensity = i * linear_wind_base;
                if (i > 3) winds[i].intensity = 3 * linear_wind_base;
                if (winds[i].intensity == wind_min) winds[i].intensity = 1;
                winds[i].recalc_wind_vector();
            }
            calculate_avg_wind_dir();
        }

        ImGui::SameLine();
        if (ImGui::Button("Max Intensity"))
        {
            is_wind = true;
            for (unsigned int i = 0; i < winds.size(); i++)
            {
                winds[i].intensity = wind_max;
                winds[i].recalc_wind_vector();
            }
            calculate_avg_wind_dir();
        }

        ImGui::SameLine();
        ImGui::Checkbox("All Angles", &all_angles);

        // Wind
        float lin_windbase_min = 0., lin_windbase_max = 35.;
        if (ImGui::SliderScalar("Linear wind speed", ImGuiDataType_Float, &linear_wind_base, &lin_windbase_min, &lin_windbase_max, "%1.f m/s"))
        {
            if (is_wind)
            {
                for (unsigned int i = 0; i < winds.size(); i++)
                {
                    winds[i].intensity = i * linear_wind_base;
                    if (i > 3) winds[i].intensity = 3 * linear_wind_base;
                    if (winds[i].intensity == wind_min) winds[i].intensity = 1;
                    winds[i] = wind_structure(winds[i].intensity, this->deg_angle[i]);
                    winds[i].recalc_wind_vector();
                }
                calculate_avg_wind_dir();
            }
        }
        ImGui::PopItemWidth();

        if (ImGui::Button("Set to 2020 Eruption Params"))
        {
            //U_0 = 200;
            //rho_0 = 250;

            winds[0].intensity = 1;
            winds[1].intensity = 14;
            winds[2].intensity = 20;
            winds[3].intensity = 30;
            winds[4].intensity = 40;
            winds[5].intensity = 58;

            this->deg_angle[0] = 0;
            this->deg_angle[1] = 30;
            this->deg_angle[2] = 330;
            this->deg_angle[3] = 90;
            this->deg_angle[4] = 120;
            this->deg_angle[5] = 135;

            for (unsigned int i = 0; i < winds.size(); i++)
            {
                winds[i] = wind_structure(winds[i].intensity, this->deg_angle[i]);
                winds[i].recalc_wind_vector();
            }
            calculate_avg_wind_dir();
        }

        ImGui::Unindent();
        ImGui::EndChild();
    }

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    ImGui::End();
}

void scene_model::set_gui_playback(gui_structure& gui)
{
    ImGui::Begin("Playback", &gui.enabled["Playback"], ImVec2(100, 73), -1.0f, ImGuiWindowFlags_NoResize);

    // Start and stop animation
    if (state == engine_state::stopped || state == engine_state::paused)
    {
        if (ImGui::ImageButton((ImTextureID)scene_model::playIcon, ImVec2(32, 32)))
        {
            timer.start();
            state = engine_state::playing;
        }
    }
    else if (state == engine_state::playing)
    {
        if (ImGui::ImageButton((ImTextureID)scene_model::pauseIcon, ImVec2(32, 32)))
        {
            timer.stop();
            state = engine_state::paused;
        }
    }

    if (state == engine_state::playing || state == engine_state::paused)
    {
        ImGui::SameLine();
        if (ImGui::ImageButton((ImTextureID)scene_model::resetIcon, ImVec2(32, 32)))
        {
            reset_simulation();
            state = engine_state::stopped;
        }
    }
    ImGui::End();
}

void scene_model::set_gui_profiler(gui_structure& gui)
{
    Plume* plume = plumes[0];
    ImGui::Begin("Profiler", &gui.enabled["Profiler"], ImGuiWindowFlags_AlwaysAutoResize);

    std::string smoke_layers_count = "Smoke Layers: " + std::to_string(plume->smoke_layers.size());
    std::string free_sphere_count = "Free Spheres: " + std::to_string(plume->free_spheres.size());
    std::string falling_sphere_count = "Falling Spheres: " + std::to_string(plume->falling_spheres.size());
    std::string stagnate_sphere_count = "Stagnate Spheres: " + std::to_string(plume->stagnate_spheres.size());
    std::string subsphere_count = "Subspheres: " + std::to_string(plume->s2_spheres.size() + plume->s3_spheres.size());

    ImGui::Text(smoke_layers_count.c_str());
    ImGui::Text(free_sphere_count.c_str());
    ImGui::Text(falling_sphere_count.c_str());
    ImGui::Text(stagnate_sphere_count.c_str());
    ImGui::Text(subsphere_count.c_str());

    ImGui::End();
}

void scene_model::keyboard_input(scene_structure& scene, GLFWwindow* window, int key, int scancode, int action, int mods)
{
    const bool key_escape = (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS);
    const bool key_space = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);

    if (key_escape)
    {
        reset_simulation();
        state = engine_state::stopped;
    }

    if (key_space)
    {
         if (state == engine_state::stopped || state == engine_state::paused)
         {
             timer.start();
             state = engine_state::playing;
         }
         else if (state == engine_state::playing)
         {
             timer.stop();
             state = engine_state::paused;
         }
    }
}
