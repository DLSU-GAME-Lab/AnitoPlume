#include "smoke.hpp"
#include "singleton/CameraManager.hpp"
#include "singleton/ShaderManager.hpp"
#include "singleton/PlumeManager.hpp"
#include "singleton/MeshManager.hpp"

using namespace vcl;

//------------------------------------------------------------
//------------------------ PROCESS ---------------------------
//------------------------------------------------------------ */

void scene_model::update()
{
    setup_terrain_preemptive();

    dt = timer.update();
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
    set_gui(gui);
    set_gui_playback(gui);
    set_gui_profiler(gui);
    t_loader.show_gui(&gui.enabled["Terrain"]);
    direction_tracker.show_gui(&gui.enabled["Direction Tracker"]);

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
    direction_tracker.initialize(max_altitude, direction_tracker_step_size);
    direction_tracker.load_data("../scenes/sources/smoke/taal_danger_zones.csv");
    direction_tracker.set_wind_direction(PlumeManager::getInstance()->getAverageWindDirection());

}

void scene_model::setup_plume_params()
{

    // Vent positions setup
    vent_positions.push_back(vec3(2500, 0, 0));
    vent_positions.push_back(vec3(5850, 5950, 0));
    vent_positions.push_back(vec3(-2000, -6000, 0));
    vent_positions.push_back(vec3(-3100, 6200, 200));
    vent_positions.push_back(vec3(5850, -6000, 0));
    
    //plumes.push_back(Plume(vent_positions[0]));
    //Plume* plume = new Plume(vent_positions[0]);
    for (int i = 0; i <vent_positions.size();i++)
    {
        PlumeManager::getInstance()->createPlume(vent_positions[i]);
    }

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
        // Display torus
        if (gui_param.display_smoke_layers) display_smoke_layers(PlumeManager::getInstance()->getPlumes()[i]);
        // billboards
        if (gui_param.display_billboards) display_billboards(PlumeManager::getInstance()->getPlumes()[i]);
        // free + stagnation spheres display
        if (gui_param.display_free_spheres) display_free_spheres(PlumeManager::getInstance()->getPlumes()[i]);
        // spheres+subspheres display (lighter)
        if (gui_param.display_spheres_with_subspheres) display_spheres_with_subspheres(PlumeManager::getInstance()->getPlumes()[i]);
        // subspheres display
        if (gui_param.display_subspheres) display_subspheres(PlumeManager::getInstance()->getPlumes()[i]);
        // falling spheres display
        if (gui_param.display_free_spheres) display_falling_spheres(PlumeManager::getInstance()->getPlumes()[i]);
        // buffer falling spheres display
        if (gui_param.display_free_spheres) display_falling_spheres_buffers(PlumeManager::getInstance()->getPlumes()[i]);
    }
    
    if (gui_param.display_tooltips == true  && camera->mode != view_mode::orbital) tip_loader.draw();

    mark_loader.draw();
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
    for (int j = 0; j < plume.getTransitionLifetime().size(); j++)
    {
        float animation = fmax(0, sinf(plume.getTransitionSpeed() * plume.getTransitionLifetime()[j]));
        float new_scaling = animation == 0 ? 4 : 2.0f + (animation * 2.0f);
        float offset = terrain_display.uniform.transform.translation.z;
        vec3 new_translation = vec3(0, 0, offset + (animation * (fabs(offset) - 2)));
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

void scene_model::reset_simulation()
{
    timer.stop();
    direction_tracker.reset_plume_positions();
    frame_count = 0;
    sim_time = 0;

    PlumeManager::getInstance()->reset();

   

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
    Plume& plume = PlumeManager::getInstance()->getPlumes()[0];
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
    ImGui::SliderScalar("Number of subspheres", ImGuiDataType_S32, &plume.subspheres_number, &spheres_min, &spheres_max);
    ImGui::SliderScalar("Number of subsubspheres", ImGuiDataType_S32, &plume.subsubspheres_number, &spheres_min, &spheres_max);
    ImGui::PopItemWidth();

    show_display_settings();
    show_wind_settings();
    show_eruption_parameters();
    // Coeffs

    //float air_inc_min = 0.5, air_inc_max = 10.;
    //ImGui::SliderScalar("Air incorporation coefficient", ImGuiDataType_Float, &air_incorporation_coeff, &air_inc_min, &air_inc_max, "%.2f");

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    ImGui::End();
}

void scene_model::show_display_settings()
{
    const float indent_width = 5;
    const float child_width = 380;

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

}

void scene_model::show_wind_settings()
{
    const float indent_width = 5;
    const float child_width = 380;

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
            intensity[i] = PlumeManager::getInstance()->getWinds()[i].intensity;
            angle[i] = PlumeManager::getInstance()->getDegAngle()[i];
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

        static int wind_intensity = PlumeManager::getInstance()->getWinds()[selected].intensity;
        if (ImGui::VSliderScalar("##Intensity Slider", ImVec2(slider_width, plot_height), ImGuiDataType_S32, &wind_intensity, &wind_min, &wind_max))
        {
            PlumeManager::getInstance()->setWindIntensity(selected, wind_intensity);
            direction_tracker.set_wind_direction(PlumeManager::getInstance()->getAverageWindDirection());
        }

        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
        ImGui::PlotLines("##Wind Intensity", intensity, wind_size, 0, "Wind Intensity (m/s)", wind_min, wind_max, ImVec2(plot_width, plot_height));
        ImGui::PopStyleColor();

        static int wind_angle = PlumeManager::getInstance()->getDegAngle()[selected];
        if (ImGui::VSliderScalar("##Angle Slider", ImVec2(slider_width, plot_height), ImGuiDataType_S32, &wind_angle, &angle_min, &angle_max))
        {
            if (all_angles)
            {
                PlumeManager::getInstance()->setAllWindAngles(wind_angle);
            }
            else
            {
                PlumeManager::getInstance()->setWindAngle(selected, wind_angle);
            }
            direction_tracker.set_wind_direction(PlumeManager::getInstance()->getAverageWindDirection());
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
                if (PlumeManager::getInstance()->getWindAlts()[i] == wind_alt)
                {
                    altitude_selected = true;
                    selected = i;
                }
            }

            if (!altitude_selected)
            {
                selected = clamp(((float)wind_alt / altitude_step) + 0.5f, 0, wind_size - 1);
                wind_alt = PlumeManager::getInstance()->getWindAlts()[selected];
            }
        }
        ImGui::PopItemWidth();
        ImGui::Unindent(indent_w);
        ImGui::Spacing();

        if (ImGui::Button("No wind"))
        {
            is_wind = false;
            PlumeManager::getInstance()->setAllWinds(wind_min, angle_min);
            direction_tracker.set_wind_direction(PlumeManager::getInstance()->getAverageWindDirection());
        }

        ImGui::SameLine();
        if (ImGui::Button("Linear Wind"))
        {
            is_wind = true;
            PlumeManager::getInstance()->setLinearWind(linear_wind_base);
            direction_tracker.set_wind_direction(PlumeManager::getInstance()->getAverageWindDirection());
        }

        ImGui::SameLine();
        if (ImGui::Button("Max Intensity"))
        {
            is_wind = true;
            PlumeManager::getInstance()->setAllWindIntensities(wind_max);
            direction_tracker.set_wind_direction(PlumeManager::getInstance()->getAverageWindDirection());
        }

        ImGui::SameLine();
        ImGui::Checkbox("All Angles", &all_angles);

        // Wind
        float lin_windbase_min = 0., lin_windbase_max = 35.;
        if (ImGui::SliderScalar("Linear wind speed", ImGuiDataType_Float, &linear_wind_base, &lin_windbase_min, &lin_windbase_max, "%1.f m/s"))
        {
            if (is_wind)
            {
                PlumeManager::getInstance()->setLinearWind(linear_wind_base);
                direction_tracker.set_wind_direction(PlumeManager::getInstance()->getAverageWindDirection());
            }
        }
        ImGui::PopItemWidth();

        if (ImGui::Button("Set to 2020 Eruption Params"))
        {
            //U_0 = 200;
            //rho_0 = 250;
            std::vector<int> intensities;
            std::vector<int> angles;

            intensities.push_back(1);
            intensities.push_back(14);
            intensities.push_back(20);
            intensities.push_back(30);
            intensities.push_back(40);
            intensities.push_back(58);

            angles.push_back(0);
            angles.push_back(30);
            angles.push_back(330);
            angles.push_back(90);
            angles.push_back(120);
            angles.push_back(135);

            for (unsigned int i = 0; i < angles.size(); i++)
            {
                PlumeManager::getInstance()->setWind(i, intensities[i], angles[i]);
            }
            direction_tracker.set_wind_direction(PlumeManager::getInstance()->getAverageWindDirection());
        }

        ImGui::Unindent();
        ImGui::EndChild();
    }
}

void scene_model::show_eruption_parameters()
{
    Plume& plume = PlumeManager::getInstance()->getPlumes()[0];
    const float indent_width = 5;
    const float child_width = 380;



    // Initial conditions
    if (ImGui::CollapsingHeader("Eruption Parameters", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::BeginChild("Parameters", ImVec2(child_width, ImGui::GetItemsLineHeightWithSpacing() * 4.25f));
        ImGui::Spacing();
        ImGui::Indent(indent_width);
        ImGui::PushItemWidth(200);

        double initial_speed_min = 0., initial_speed_max = 200.;
        ImGui::SliderScalar("Initial plume speed", ImGuiDataType_Double, &fU0, &initial_speed_min, &initial_speed_max, "%.2f m/s");
        plume.setU0(fU0);
        double initial_density_min = 150., initial_density_max = 250.;
        ImGui::SliderScalar("Initial plume density", ImGuiDataType_Double, &fRho0, &initial_density_min, &initial_density_max, "%.2f kg/m3");
        plume.setRho0(fRho0);
        double vent_ray_min = 50., vent_ray_max = 200.;
        ImGui::SliderScalar("Vent radius", ImGuiDataType_Double, &fR0, &vent_ray_min, &vent_ray_max, "%.2f m");
        plume.setR0(fR0);

        double vent_altitude_min = 0., vent_altitude_max = 8000.;
        ImGui::SliderScalar("Vent altitude", ImGuiDataType_Double, &fZ0, &vent_altitude_min, &vent_altitude_max, "%.2f m");
        plume.setZ0(fZ0);

        ImGui::PopItemWidth();
        ImGui::Unindent();
        ImGui::EndChild();

    }

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
