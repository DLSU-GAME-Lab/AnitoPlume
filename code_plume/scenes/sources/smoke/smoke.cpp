#include "smoke.hpp"
#include "singleton/CameraManager.hpp"
#include "singleton/ShaderManager.hpp"
#include "singleton/PlumeManager.hpp"
#include "singleton/MeshManager.hpp"
#include "singleton/GUIManager.hpp"
#include "singleton/PlumeTracker.hpp"

using namespace vcl;

//------------------------------------------------------------
//------------------------ PROCESS ---------------------------
//------------------------------------------------------------ */

void scene_model::update()
{
    setup_terrain_preemptive();

    dt = timer.update();
    timer.scale = sim_input->getTimerScale();
    // Force constant time step
    t_step = dt <= 1e-6f ? 0.0f : timer.scale * 0.002f; //0.0003f

    PlumeManager::getInstance()->setTStep(t_step);
   
    if (!replay)
    {
        sim_time += t_step;
        //PlumeManager::getInstance()->getPlumes()[i]->remove_colliding_smoke();
        PlumeManager::getInstance()->removeSmokeLayers();

        for (unsigned int nb_steps_per_frame = 0; nb_steps_per_frame < 10; nb_steps_per_frame++)
        {
            PlumeManager::getInstance()->update(frame_count);
            frame_count++;
        }
    }
}

void scene_model::frame_draw(scene_structure& scene, gui_structure& gui)
{
    set_gui_playback(gui);
    set_gui_profiler(gui);
    t_loader.show_gui(&gui.enabled["Terrain"]);

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

    gui.show_frame_camera = false; std::cout << "replay becomes false 0" << std::endl;
    gui.enabled["Simulator Input"] = true;
    gui.enabled["Direction Tracker"] = true;
    gui.enabled["Playback"] = true;
    gui.enabled["Profiler"] = true;
    gui.enabled["Terrain"] = true;

    setup_plume_params();

    // Meshes setup



    MeshManager::getInstance()->getMesh("Generic_Sphere")->texture_id = scene.texture_white;
    //generic_torus_mesh = vcl::mesh_primitive_torus(1.,1.,{0,0,0}, {0,0,-1});
    
    MeshManager::getInstance()->getMesh("Generic_Torus")->uniform.color = {1,0.5,0};
    MeshManager::getInstance()->getMesh("Generic_Torus")->shader = ShaderManager::getInstance()->getShader("mesh");
    MeshManager::getInstance()->getMesh("Generic_Torus")->texture_id = scene.texture_white;
    MeshManager::getInstance()->getMesh("Generic_Torus")->uniform.color_alpha = 0.6f;
    //texture_smoke_id = create_texture_gpu( image_load_png("../scenes/sources/smoke/images/texture_panache.png") );
    pauseIcon = create_texture_gpu(image_load_png("../scenes/sources/smoke/images/pause_icon.png"));
    playIcon = create_texture_gpu(image_load_png("../scenes/sources/smoke/images/play_icon.png"));
    resetIcon = create_texture_gpu(image_load_png("../scenes/sources/smoke/images/undo_icon.png"));

    smoke_texture = create_texture_gpu(image_load_png("../scenes/sources/smoke/smoke_tex/smoke-tex-0.png"));
    MeshManager::getInstance()->getMesh("Quad")->uniform.shading.ambiant = 1.0;
    MeshManager::getInstance()->getMesh("Quad")->uniform.shading.diffuse = 0.0;
    MeshManager::getInstance()->getMesh("Quad")->uniform.shading.specular = 0.0;

    PlumeManager::getInstance()->setupTransitionValues(20, 5.f, .2f); 




    //skysphere = mesh_drawable(mesh_primitive_sphere(100.0f));
    MeshManager::getInstance()->getMesh("Skysphere")->texture_id = create_texture_gpu(image_load_png("../scenes/sources/smoke/Skydome/Skysphere_Tex.png"));
    MeshManager::getInstance()->getMesh("Skysphere")->uniform.color = { 1, 1, 1 };
    MeshManager::getInstance()->getMesh("Skysphere")->uniform.shading.specular = 100.0f;
    MeshManager::getInstance()->getMesh("Skysphere")->uniform.shading.ambiant = 1.0f;
    MeshManager::getInstance()->getMesh("Skysphere")->uniform.shading.diffuse = 1.0f;
    MeshManager::getInstance()->getMesh("Skysphere")->uniform.transform.rotation = rotation_from_axis_angle_mat3({ 1.0f,0,0 }, 3.14f / 2.0f);
    MeshManager::getInstance()->getMesh("Skysphere")->uniform.transform.scaling = 1.0f;
    MeshManager::getInstance()->getMesh("Skysphere")->uniform.transform.translation = { 0,0,0 };



    //sampling subpheres
    
   

    //subspheres_display = mesh_drawable(m) ;

    MeshManager::getInstance()->getMesh("Subsphere_Disp")->texture_id = scene.texture_white;
    MeshManager::getInstance()->getMesh("Subsphere_Disp")->uniform.color = {0.6,0.6,0.55};
    MeshManager::getInstance()->getMesh("Subsphere_Disp")->uniform.shading.ambiant = 0.7f;
    MeshManager::getInstance()->getMesh("Subsphere_Disp")->uniform.shading.diffuse = 0.3f;
    MeshManager::getInstance()->getMesh("Subsphere_Disp")->uniform.shading.specular = 0.0f;
    
    
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
        PlumeManager::getInstance()->getDegAngle().push_back(0);
    }

    is_wind = false;
    linear_wind_base = 15.;
    selected = 0;
    wind_alt = 0;
    fU0 = 150;
    fRho0 = 200;
    fR0 = 100;
    fZ0 = 0;

    // Direction tracker setup
    direction_tracker_step = 1000.0f;
    direction_tracker_step_size = 20;
    PlumeTracker::getInstance()->setWindDirection(PlumeManager::getInstance()->getAverageWindDirection());
    sim_input = (SimulatorInputScreen*)GUIManager::getInstance()->getGUIScreen("Simulator Input");
}

void scene_model::setup_plume_params()
{

    // Vent positions setup
    vent_names[0] = "Taal Main Crater";
    vent_names[1] = "Pira-piraso";
    vent_names[2] = "Binintiang Munti";
    vent_names[3] = "Binintiang Malaki";
    vent_names[4] = "Calauit Point";

    vent_positions[0] = vec3(2500, 0, 0);
    vent_positions[1] = vec3(5850, 5950, 0);
    vent_positions[2] = vec3(-2000, -6000, 0);
    vent_positions[3] = vec3(-3100, 6200, 200);
    vent_positions[4] = vec3(5850, -6000, 0);

    erupt_params.push_back(EruptionParams{ 150., 0, 100, 200, 1000, 500 });
    erupt_params.push_back(EruptionParams{ 150., 0, 100, 200, 250, 125 });
    erupt_params.push_back(EruptionParams{ 150., 0, 100, 200, 500, 250 });
    erupt_params.push_back(EruptionParams{ 150., 0, 100, 200, 250, 125 });
    erupt_params.push_back(EruptionParams{ 150., 0, 100, 200, 1000, 500 });
    
    for (int i = 0; i < 5; i++)
    {
        PlumeManager::getInstance()->createPlume(i, vent_names[i], vent_positions[i], erupt_params[i]);
    }
    PlumeManager::getInstance()->setToUpdate(0, true);
}


void scene_model::display(scene_structure& scene)
{
    camera_scene* camera = CameraManager::getInstance()->getCamera();
    float ratio = 100;

    if (camera->sky_enabled)
        MeshManager::getInstance()->getMesh("Skysphere")->draw_sky(*camera, ShaderManager::getInstance()->getShader("sky_mesh"), MeshManager::getInstance()->getMesh("Skysphere")->texture_id);
        //draw_sky(sky_sphere, *camera, shaders["sky_mesh"], scene.texture_white);

    if (terrain_display.data.number_triangles > 0)
    {
        //draw(terrain_display, *camera, ShaderManager::getInstance()->getShader("mesh"), true);

        terrain_display.texture_id = t_loader.current_tex_id;
        terrain_display.norm_tex_id = t_loader.current_norm_id;

        terrain_display.draw_mix(*camera, ShaderManager::getInstance()->getShader("mesh_mix"), terrain_display.texture_id, terrain_display.norm_tex_id, decal, 1);
    }
    //draw(terrain, *camera, shaders["wireframe"]);

    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i = 0; i < PlumeManager::getInstance()->getPlumes().size(); i++)
    {
        // billboards
        if (sim_input->getDisplayBillboards()) display_billboards(PlumeManager::getInstance()->getPlumes()[i]);
        // Display torus
        if (sim_input->getDisplaySmokeLayers()) display_smoke_layers(PlumeManager::getInstance()->getPlumes()[i]);
        // free + stagnation spheres display
        if (sim_input->getDisplayFreeSpheres()) display_free_spheres(PlumeManager::getInstance()->getPlumes()[i]);
        // spheres+subspheres display (lighter)
        if (sim_input->getDisplaySpheresWithSubspheres()) display_spheres_with_subspheres(PlumeManager::getInstance()->getPlumes()[i]);
        // subspheres display
        if (sim_input->getDisplaySubspheres()) display_subspheres(PlumeManager::getInstance()->getPlumes()[i]);
        // falling spheres display
        if (sim_input->getDisplayFreeSpheres()) display_falling_spheres(PlumeManager::getInstance()->getPlumes()[i]);
        // buffer falling spheres display
        if (sim_input->getDisplayFreeSpheres()) display_falling_spheres_buffers(PlumeManager::getInstance()->getPlumes()[i]);
    }
    
    if (gui_param.display_tooltips == true  && camera->mode != view_mode::orbital) tip_loader.draw();

    if (sim_input->getDisplayLandmarks()) mark_loader.draw();
}

#pragma region Unique Display

void scene_model::display_smoke_layers(Plume& plume)
{
    camera_scene* camera = CameraManager::getInstance()->getCamera();
    float ratio = 100;

    for (unsigned int i = 0; i < plume.smoke_layers.size(); i++)
    {
        smoke_layer lay = plume.smoke_layers[i];
        MeshManager::getInstance()->getMesh("Generic_Torus")->uniform.transform.scaling = lay.r / ratio;
        MeshManager::getInstance()->getMesh("Generic_Torus")->uniform.transform.translation = vec3(lay.center.x / ratio - 25, lay.center.y / ratio, lay.center.z / ratio - 2);
        MeshManager::getInstance()->getMesh("Generic_Torus")->uniform.transform.rotation = rotation_from_axis_angle_mat3(lay.theta_axis, lay.theta - 3.14 / 2.0);
        MeshManager::getInstance()->getMesh("Generic_Torus")->shader = ShaderManager::getInstance()->getShader("mesh");
        MeshManager::getInstance()->getMesh("Generic_Torus")->draw(*camera);
    }
}

void scene_model::display_billboards(Plume& plume)
{
    camera_scene* camera = CameraManager::getInstance()->getCamera();
    float ratio = 100;

    glDepthMask(false);

    // transition smoke
    if (PlumeManager::getInstance()->getToUpdate(plume.getID()))
    {
        for (int j = 0; j < plume.getTransitionLifetime().size(); j++)
        {
            float animation = fmax(0, sinf(plume.getTransitionSpeed() * plume.getTransitionLifetime()[j]));
            float new_scaling = animation == 0 ? 4 : 2.0f + (animation * 2.0f);
            float offset = terrain_display.uniform.transform.translation.z;
            vec3 new_translation = vec3(plume.getPosition().x / ratio - 25, plume.getPosition().y / ratio, offset + (animation * (fabs(offset) - 2)));
            float var = vcl::perlin(j, 2);

            MeshManager::getInstance()->getMesh("Quad")->uniform.transform.rotation = rotation_from_axis_angle_mat3(camera->orientation.col(2), plume.getTransitionSpeed() * plume.getTransitionLifetime()[j] * var) * camera->orientation;
            MeshManager::getInstance()->getMesh("Quad")->uniform.transform.translation = new_translation;
            MeshManager::getInstance()->getMesh("Quad")->uniform.transform.scaling = new_scaling * 1.3;
            MeshManager::getInstance()->getMesh("Quad")->uniform.color = { 0.3f,0.3f,0.3f };
            MeshManager::getInstance()->getMesh("Quad")->uniform.color_alpha = (0.8 + 0.3f * (2 * var - 1.0f)) * fmax(0.2f, animation);
            MeshManager::getInstance()->getMesh("Quad")->shader = ShaderManager::getInstance()->getShader("mesh");
            MeshManager::getInstance()->getMesh("Quad")->texture_id = smoke_texture;

            MeshManager::getInstance()->getMesh("Quad")->draw(*camera);
        }
    }

    for (unsigned int j = 0; j < plume.free_spheres.size(); j++)
    {
        mat3 const R = rotation_from_axis_angle_mat3(plume.free_spheres[j].rotation_axis, plume.free_spheres[j].current_angle);
        float new_scaling = plume.free_spheres[j].r / ratio;
        //if (j==0) std::cout << new_scaling << std::endl;
        vec3 new_translation = vec3(plume.free_spheres[j].center.x / ratio - 25, plume.free_spheres[j].center.y / ratio, plume.free_spheres[j].center.z / ratio - 2);
     /*   MeshManager::getInstance()->getMesh("Generic_Sphere")->uniform.transform.translation = new_translation;
        MeshManager::getInstance()->getMesh("Generic_Sphere")->uniform.transform.scaling = new_scaling;
        MeshManager::getInstance()->getMesh("Generic_Sphere")->uniform.transform.rotation = R;
        MeshManager::getInstance()->getMesh("Generic_Sphere")->uniform.color = { 1,1,1 };
        MeshManager::getInstance()->getMesh("Generic_Sphere")->shader = ShaderManager::getInstance()->getShader("mesh");*/

        float var = vcl::perlin(plume.free_spheres[j].id, 2);

        //MeshManager::getInstance()->getMesh("Quad")->uniform.transform.rotation = rotation_from_axis_angle_mat3(camera->orientation.col(2), free_spheres[j].current_angle * dot(free_spheres[j].rotation_axis, camera->orientation.col(2)) * 1.5f *(1+0.3*var) + 2.2145*j*j) * camera->orientation;
        MeshManager::getInstance()->getMesh("Quad")->uniform.transform.rotation = rotation_from_axis_angle_mat3(camera->orientation.col(2), plume.free_spheres[j].id * var) * camera->orientation;
        MeshManager::getInstance()->getMesh("Quad")->uniform.transform.translation = new_translation;
        MeshManager::getInstance()->getMesh("Quad")->uniform.transform.scaling = new_scaling * 1.3;
        MeshManager::getInstance()->getMesh("Quad")->uniform.color_alpha = 0.8 + 0.3f * (2 * var - 1.0f);

        float l = (plume.free_spheres[j].lifetime / 120) + 0.3f;
        if (l > 1) l = 1;
        MeshManager::getInstance()->getMesh("Quad")->uniform.color = { l,l,l };

        float end_fade = 1.0f;
        if (plume.free_spheres[j].lifetime >= plume.min_lifetime)
            end_fade -= (plume.free_spheres[j].lifetime - plume.min_lifetime) / (plume.max_lifetime - plume.min_lifetime);
        MeshManager::getInstance()->getMesh("Quad")->uniform.color_alpha *= end_fade;
        MeshManager::getInstance()->getMesh("Quad")->shader = ShaderManager::getInstance()->getShader("mesh");

        MeshManager::getInstance()->getMesh("Quad")->draw(*camera);
    }
    glDepthMask(true);
}

void scene_model::display_free_spheres(Plume& plume)
{
    camera_scene* camera = CameraManager::getInstance()->getCamera();
    float ratio = 100;

    for (unsigned int j = 0; j < plume.free_spheres.size(); j++)
    {
        //if (!free_spheres[j].falling)
        mat3 const R = rotation_from_axis_angle_mat3(plume.free_spheres[j].rotation_axis, plume.free_spheres[j].current_angle);
        float r = plume.free_spheres[j].r / ratio;
        vec3 t = vec3(plume.free_spheres[j].center.x / ratio - 25, plume.free_spheres[j].center.y / ratio, plume.free_spheres[j].center.z / ratio - 2);
        float rho = plume.free_spheres[j].rho;
        float disp_rho = 1. - rho;
        if (disp_rho < 0) disp_rho = 0.;
        disp_rho = 1.;

        MeshManager::getInstance()->getMesh("Generic_Sphere")->uniform.transform.translation = t;
        MeshManager::getInstance()->getMesh("Generic_Sphere")->uniform.transform.scaling = r;
        MeshManager::getInstance()->getMesh("Generic_Sphere")->uniform.transform.rotation = R;
        MeshManager::getInstance()->getMesh("Generic_Sphere")->uniform.color = { disp_rho,disp_rho,disp_rho };
        MeshManager::getInstance()->getMesh("Generic_Sphere")->shader = ShaderManager::getInstance()->getShader("mesh");
        MeshManager::getInstance()->getMesh("Generic_Sphere")->draw(*camera);
    }
}

void scene_model::display_spheres_with_subspheres(Plume& plume)
{
    camera_scene* camera = CameraManager::getInstance()->getCamera();
    float ratio = 100;
    for (unsigned int j = 0; j < plume.free_spheres.size(); j++)
    {
        //if (!free_spheres[j].falling)
        mat3 const R = rotation_from_axis_angle_mat3(plume.free_spheres[j].rotation_axis, plume.free_spheres[j].current_angle);
        float r = plume.free_spheres[j].r / ratio;
        vec3 t = vec3(plume.free_spheres[j].center.x / ratio - 25, plume.free_spheres[j].center.y / ratio, plume.free_spheres[j].center.z / ratio - 2);
        float rho = plume.free_spheres[j].rho;
        float disp_rho = 1. - rho;
        if (disp_rho < 0) disp_rho = 0.;
        disp_rho = 1.;

        MeshManager::getInstance()->getMesh("Subsphere_Disp")->uniform.transform.translation = t;
        MeshManager::getInstance()->getMesh("Subsphere_Disp")->uniform.transform.scaling = r;
        MeshManager::getInstance()->getMesh("Subsphere_Disp")->uniform.transform.rotation = R;
        MeshManager::getInstance()->getMesh("Subsphere_Disp")->uniform.color = { disp_rho,disp_rho,disp_rho };
        MeshManager::getInstance()->getMesh("Subsphere_Disp")->shader = ShaderManager::getInstance()->getShader("mesh");

        MeshManager::getInstance()->getMesh("Subsphere_Disp")->draw(*camera);
    }
}

void scene_model::display_subspheres(Plume& plume)
{
    camera_scene* camera = CameraManager::getInstance()->getCamera();
    float ratio = 100;
    for (unsigned int j = 0; j < plume.s2_spheres.size(); j++)
    {
        if (!plume.free_spheres[plume.s2_spheres[j].parent_id].falling)
        {
            float r = plume.s2_spheres[j].r / ratio;
            vec3 t = plume.s2_spheres[j].center / ratio;
            float rho = plume.free_spheres[plume.s2_spheres[j].parent_id].rho;
            float disp_rho = 1. - rho;
            if (disp_rho < 0) disp_rho = 0.;
            disp_rho = 1.;

            MeshManager::getInstance()->getMesh("Generic_Sphere")->uniform.transform.translation = t;
            MeshManager::getInstance()->getMesh("Generic_Sphere")->uniform.transform.scaling = r;
            MeshManager::getInstance()->getMesh("Generic_Sphere")->uniform.color = { disp_rho,disp_rho,disp_rho };
            MeshManager::getInstance()->getMesh("Generic_Sphere")->shader = ShaderManager::getInstance()->getShader("mesh");
            MeshManager::getInstance()->getMesh("Generic_Sphere")->draw(*camera);
        }
    }
}

void scene_model::display_falling_spheres(Plume& plume)
{
    camera_scene* camera = CameraManager::getInstance()->getCamera();
    float ratio = 100;

    for (unsigned int j = 0; j < plume.falling_spheres.size(); j++)
    {
        float new_scaling = plume.falling_spheres[j].r / ratio;
        vec3 new_translation = { plume.falling_spheres[j].center.x / ratio, plume.falling_spheres[j].center.y / ratio, plume.falling_spheres[j].center.z / ratio - 2 };
        MeshManager::getInstance()->getMesh("Generic_Sphere")->uniform.transform.translation = new_translation;
        MeshManager::getInstance()->getMesh("Generic_Sphere")->uniform.transform.scaling = new_scaling;
        MeshManager::getInstance()->getMesh("Generic_Sphere")->uniform.transform.rotation = mat3::identity();
        if (plume.falling_spheres[j].falling_under_atm_rho) MeshManager::getInstance()->getMesh("Generic_Sphere")->uniform.color = { 1,0,0 };
        else MeshManager::getInstance()->getMesh("Generic_Sphere")->uniform.color = { 1,1,1 };
        MeshManager::getInstance()->getMesh("Generic_Sphere")->shader = ShaderManager::getInstance()->getShader("mesh");
        MeshManager::getInstance()->getMesh("Generic_Sphere")->draw(*camera);
    }
}

void scene_model::display_falling_spheres_buffers(Plume& plume)
{
    camera_scene* camera = CameraManager::getInstance()->getCamera();
    float ratio = 100;

    for (unsigned int k = 0; k < plume.falling_spheres_buffers.size(); k++)
    {
        for (unsigned int j = 0; j < plume.falling_spheres_buffers[k].size(); j++)
        {
            float new_scaling = plume.falling_spheres_buffers[k][j].r / ratio;
            vec3 new_translation = { plume.falling_spheres_buffers[k][j].center.x / ratio, plume.falling_spheres_buffers[k][j].center.y / ratio, plume.falling_spheres_buffers[k][j].center.z / ratio + 5 };
            MeshManager::getInstance()->getMesh("Generic_Sphere")->uniform.transform.translation = new_translation;
            MeshManager::getInstance()->getMesh("Generic_Sphere")->uniform.transform.scaling = new_scaling;
            MeshManager::getInstance()->getMesh("Generic_Sphere")->uniform.transform.rotation = mat3::identity();
            if (plume.falling_spheres_buffers[k][j].falling_under_atm_rho) MeshManager::getInstance()->getMesh("Generic_Sphere")->uniform.color = { 1,0,0 };
            else MeshManager::getInstance()->getMesh("Generic_Sphere")->uniform.color = { 1,1,1 };
            MeshManager::getInstance()->getMesh("Generic_Sphere")->shader = ShaderManager::getInstance()->getShader("mesh");
            MeshManager::getInstance()->getMesh("Generic_Sphere")->draw(*camera);
        }
    }
}

#pragma endregion

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

void scene_model::set_gui_playback(gui_structure& gui)
{
    ImGui::Begin("Playback", &gui.enabled["Playback"], ImVec2(100, 73), -1.0f, ImGuiWindowFlags_NoResize);

    // Start and stop animation
    SimulatorState state = PlumeManager::getInstance()->getState();
    if (state == SimulatorState::Stopped || state == SimulatorState::Paused)
    {
        if (ImGui::ImageButton((ImTextureID)scene_model::playIcon, ImVec2(32, 32)))
        {
            play();
        }
    }
    else if (state == SimulatorState::Playing)
    {
        if (ImGui::ImageButton((ImTextureID)scene_model::pauseIcon, ImVec2(32, 32)))
        {
            pause();
        }
    }

    if (state == SimulatorState::Playing || state == SimulatorState::Paused)
    {
        ImGui::SameLine();
        if (ImGui::ImageButton((ImTextureID)scene_model::resetIcon, ImVec2(32, 32)))
        {
            stop();
        }
    }
    ImGui::End();
}

void scene_model::set_gui_profiler(gui_structure& gui)
{
    Plume& plume = PlumeManager::getInstance()->getPlumes()[0];
    ImGui::Begin("Profiler", &gui.enabled["Profiler"], ImGuiWindowFlags_AlwaysAutoResize);

    std::string smoke_layers_count = "Smoke Layers: " + std::to_string(plume.smoke_layers.size());
    std::string free_sphere_count = "Free Spheres: " + std::to_string(plume.free_spheres.size());
    std::string falling_sphere_count = "Falling Spheres: " + std::to_string(plume.falling_spheres.size());
    std::string stagnate_sphere_count = "Stagnate Spheres: " + std::to_string(plume.stagnate_spheres.size());
    std::string subsphere_count = "Subspheres: " + std::to_string(plume.s2_spheres.size() + plume.s3_spheres.size());

    ImGui::Text(smoke_layers_count.c_str());
    ImGui::Text(free_sphere_count.c_str());
    ImGui::Text(falling_sphere_count.c_str());
    ImGui::Text(stagnate_sphere_count.c_str());
    ImGui::Text(subsphere_count.c_str());

    ImGui::End();
}

void scene_model::stop()
{
    timer.stop();

    PlumeManager::getInstance()->stopSimulation();
    PlumeTracker::getInstance()->resetPlumePositions();
    sim_input->resetEruptOnPlay();

    frame_count = 0;
    sim_time = 0;
    frame_replay = 0;
    replay = false;
    export_data = false;
    state = engine_state::stopped;
}

void scene_model::play()
{
    timer.start();
    PlumeManager::getInstance()->playSimulation();
    state = engine_state::playing;
}

void scene_model::pause()
{
    timer.stop();
    PlumeManager::getInstance()->pauseSimulation();
    state = engine_state::paused;
}

void scene_model::keyboard_input(scene_structure& scene, GLFWwindow* window, int key, int scancode, int action, int mods)
{
    const bool key_escape = (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS);
    const bool key_space = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);

    if (key_escape)
    {
        stop();
    }

    if (key_space)
    {
         if (state == engine_state::stopped || state == engine_state::paused)
         {
             play();
         }
         else if (state == engine_state::playing)
         {
             pause();
         }
    }
}
