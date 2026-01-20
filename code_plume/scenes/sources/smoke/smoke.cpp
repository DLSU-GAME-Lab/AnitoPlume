#include "smoke.hpp"
#include "singleton/CameraManager.hpp"
#include "singleton/ShaderManager.hpp"
#include "singleton/TextureManager.hpp"
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
    PlumeManager::getInstance()->update();
}

void scene_model::frame_draw()
{
    display();
}

//------------------------------------------------------------
//------------------------- SETUP ----------------------------
//------------------------------------------------------------ */

void scene_model::setup_resources()
{
    //Textures
    TextureManager::getInstance()->load("smoke", "../assets/smoke_tex/smoke-tex-0.png");
    TextureManager::getInstance()->load("skysphere", "../assets/Skydome/Skysphere_Tex.png");

    //Meshes
    MeshManager::getInstance()->loadPrimitive("Sphere", vcl::mesh_primitive_sphere());
    MeshManager::getInstance()->loadPrimitive("Subspheres", vcl::mesh_primitive_subspheres());
    MeshManager::getInstance()->loadPrimitive("Torus", mesh_primitive_torus(2.5f, { 0,0,1.5 }, { 0,0,-1.5 }, 30, 30));
    MeshManager::getInstance()->loadPrimitive("Quad", mesh_primitive_quad({ -1,-1,0 }, { 1,-1,0 }, { 1,1,0 }, { -1,1,0 }));
    MeshManager::getInstance()->load("Skysphere", "../assets/Skydome/Taal_Skydome.obj");
}

void scene_model::setup_data()
{
    srand(time(0));

    t_loader.mesh_shader = ShaderManager::getInstance()->getShader("mesh");

    const char* years[]{ "2023", "2021", "2019", "2016" };
    for (int i = 0; i < 4; i++)
    {
        std::string year = years[i];
        std::string texture_name = "Taal_Texture_" + year;
        std::string texture_path = "../assets/textures/" + texture_name + ".png";
        TextureManager::getInstance()->load(texture_name, texture_path);
    }
    for (int i = 0; i < 4; i++)
    {
        std::string year = years[i];
        std::string normal_name = "Taal_Normal_" + year;
        std::string normal_path = "../assets/textures/" + normal_name + ".png";
        TextureManager::getInstance()->load(normal_name, normal_path);
    }

    float seed = time(0);
    srand(seed);

    setup_plume_params();

    // Meshes setup
    skysphere_display = MeshManager::getInstance()->getMesh("Skysphere");
    torus_display = MeshManager::getInstance()->getMesh("Torus");
    quad_display = MeshManager::getInstance()->getMesh("Quad");
    sphere_display = MeshManager::getInstance()->getMesh("Sphere");
    subspheres_display = MeshManager::getInstance()->getMesh("Subspheres");
    sphere_display->texture_id = TextureManager::getInstance()->getTexture("white");
    
    torus_display->uniform.color = {1,0.5,0};
    torus_display->shader = ShaderManager::getInstance()->getShader("mesh");
    torus_display->texture_id = TextureManager::getInstance()->getTexture("white");
    torus_display->uniform.color_alpha = 0.6f;

    smoke_texture = TextureManager::getInstance()->getTexture("smoke");
    quad_display->uniform.shading.ambiant = 1.0;
    quad_display->uniform.shading.diffuse = 0.0;
    quad_display->uniform.shading.specular = 0.0;

    PlumeManager::getInstance()->setupTransitionValues(20, 5.f, .2f); 

    //skysphere = mesh_drawable(mesh_primitive_sphere(100.0f));
    skysphere_display->texture_id = TextureManager::getInstance()->getTexture("skysphere");
    skysphere_display->uniform.color = { 1, 1, 1 };
    skysphere_display->uniform.shading.specular = 100.0f;
    skysphere_display->uniform.shading.ambiant = 1.0f;
    skysphere_display->uniform.shading.diffuse = 1.0f;
    skysphere_display->uniform.transform.rotation = rotation_from_axis_angle_mat3({ 1.0f,0,0 }, 3.14f / 2.0f);
    skysphere_display->uniform.transform.scaling = 1.0f;
    skysphere_display->uniform.transform.translation = { 0,0,0 };

    //sampling subpheres
    subspheres_display->texture_id = TextureManager::getInstance()->getTexture("white");
    subspheres_display->uniform.color = {0.6,0.6,0.55};
    subspheres_display->uniform.shading.ambiant = 0.7f;
    subspheres_display->uniform.shading.diffuse = 0.3f;
    subspheres_display->uniform.shading.specular = 0.0f;
    
    tip_loader.setup_tooltips();
    mark_loader.setup_landmarks();

    //load terrain
    t_loader.load_terrain("Taal-Spherical-2_0.obj", "Taal_Texture_2023.png");

    terrain_display = t_loader.terrain;
    terrain_display.uniform.transform.scaling = .25f;
    //terrain_display.texture_id = create_texture_gpu(image_load_png("../assets/terrains/Taal_Texture_BaseColor_2016.png"));
    //terrain_display.norm_tex_id = add_normal_map(image_load_png("../assets/textures/Taal_Texture_normal_2024.png"));
    terrain_display.uniform.color = { 1,1,1 };
	PlumeManager::getInstance()->setupTerrainStruct(
        t_loader.mesh_terrain.position,
        t_loader.mesh_terrain.normal,
        t_loader.mesh_terrain
    );

    fU0 = 150;
    fRho0 = 200;
    fR0 = 100;
    fZ0 = 0;

    // Direction tracker setup
    PlumeTracker::getInstance()->setWindDirection(PlumeManager::getInstance()->getAverageWindDirection());
    sim_input_screen = (SimulatorInputScreen*)GUIManager::getInstance()->getGUIScreen("Simulator Input");
    display_screen = (DisplaySettingsScreen*)GUIManager::getInstance()->getGUIScreen("Display Settings");
    display_screen->initialize();
}

void scene_model::setup_plume_params()
{
    // Vent positions setup
    vent_names[0] = "Taal Main Crater";
    vent_names[1] = "Pira-piraso";
    vent_names[2] = "Binintiang Munti";
    vent_names[3] = "Binintiang Malaki";
    //vent_names[4] = "Calauit Point";

    vent_positions[0] = vec3(2500, 0, 0);
    vent_positions[1] = vec3(5850, 5950, 0);
    vent_positions[2] = vec3(-2000, -6000, 0);
    vent_positions[3] = vec3(-3100, 6200, 200);
    //vent_positions[4] = vec3(5850, -6000, 0);

    erupt_params.push_back(EruptionParams{ 150., 0., 100., 200., 500. });
    erupt_params.push_back(EruptionParams{ 50.,  0.,  20.,  50., 150. });
    erupt_params.push_back(EruptionParams{ 100., 0.,  50., 100., 250. });
    erupt_params.push_back(EruptionParams{ 50.,  0.,  20.,  50., 150. });
    //erupt_params.push_back(EruptionParams{ 150., 0., 100., 200., 500. });

    for (int i = 0; i < erupt_params.size(); i++)
    {
        PlumeManager::getInstance()->createPlume(i, vent_names[i], vent_positions[i], erupt_params[i]);
    }
    PlumeManager::getInstance()->setToUpdate(0, true);
}


void scene_model::display()
{
    camera_scene* camera = CameraManager::getInstance()->getCamera();
    float ratio = 100;

    if (camera->sky_enabled)
        skysphere_display->draw_sky(*camera, ShaderManager::getInstance()->getShader("sky_mesh"), skysphere_display->texture_id);
        //draw_sky(sky_sphere, *camera, shaders["sky_mesh"], TextureManager::getInstance()->getTexture("white"));

    if (terrain_display.data.number_triangles > 0)
    {
        //draw(terrain_display, *camera, ShaderManager::getInstance()->getShader("mesh"), true);

        terrain_display.texture_id = display_screen->getCurrentTex();
        terrain_display.norm_tex_id = display_screen->getCurrentNormTex();

        terrain_display.draw_mix(*camera, ShaderManager::getInstance()->getShader("mesh_mix"), terrain_display.texture_id, terrain_display.norm_tex_id, decal, 1);
    }
    //draw(terrain, *camera, shaders["wireframe"]);

    glBindTexture(GL_TEXTURE_2D, TextureManager::getInstance()->getTexture("white"));
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    vcl::mat4 cam_mat = CameraManager::getInstance()->getCamera()->camera_matrix();
    vcl::vec3 cam_pos = { cam_mat.xw * ratio - 25, cam_mat.yw * ratio, cam_mat.zw * ratio - 2 };
    if (cam_pos.x != last_cam_pos.x || cam_pos.y != last_cam_pos.y || cam_pos.z != last_cam_pos.z)
    {
        last_cam_pos = cam_pos;
        PlumeManager::getInstance()->sortNearestPlumes(cam_pos);
    }

    for (int i = 0; i < PlumeManager::getInstance()->getSortedPlumes().size(); i++)
    {
        // billboards
        if (display_screen->getDisplayBillboards()) display_billboards(PlumeManager::getInstance()->getSortedPlumes()[i]);
        // Display torus
        if (display_screen->getDisplaySmokeLayers()) display_smoke_layers(PlumeManager::getInstance()->getSortedPlumes()[i]);
        // free + stagnation spheres display
        if (display_screen->getDisplayFreeSpheres()) display_free_spheres(PlumeManager::getInstance()->getSortedPlumes()[i]);
        // spheres+subspheres display (lighter)
        if (display_screen->getDisplaySpheresWithSubspheres()) display_spheres_with_subspheres(PlumeManager::getInstance()->getSortedPlumes()[i]);
        // subspheres display
        if (display_screen->getDisplaySubspheres()) display_subspheres(PlumeManager::getInstance()->getSortedPlumes()[i]);
        // falling spheres display
        if (display_screen->getDisplayFreeSpheres()) display_falling_spheres(PlumeManager::getInstance()->getSortedPlumes()[i]);
        // buffer falling spheres display
        if (display_screen->getDisplayFreeSpheres()) display_falling_spheres_buffers(PlumeManager::getInstance()->getSortedPlumes()[i]);
    }
    
    if (display_screen->getDisplayTooltips() == true && camera->mode != view_mode::orbital) tip_loader.draw();

    if (display_screen->getDisplayLandmarks()) mark_loader.draw();
}

#pragma region Unique Display

void scene_model::display_smoke_layers(Plume* plume)
{
    camera_scene* camera = CameraManager::getInstance()->getCamera();
    float ratio = 100;

    for (unsigned int i = 0; i < plume->smoke_layers.size(); i++)
    {
        smoke_layer lay = plume->smoke_layers[i];
        torus_display->uniform.transform.scaling = lay.r / ratio;
        torus_display->uniform.transform.translation = vec3(lay.center.x / ratio - 25, lay.center.y / ratio, lay.center.z / ratio - 2);
        torus_display->uniform.transform.rotation = rotation_from_axis_angle_mat3(lay.theta_axis, lay.theta - 3.14 / 2.0);
        torus_display->shader = ShaderManager::getInstance()->getShader("mesh");
        torus_display->draw(*camera);
    }
}

void scene_model::display_billboards(Plume* plume)
{
    camera_scene* camera = CameraManager::getInstance()->getCamera();
    float ratio = 100;

    glDepthMask(false);

    // transition smoke
    if (PlumeManager::getInstance()->getToUpdate(plume->getID()))
    {
        for (int j = 0; j < plume->getTransitionLifetime().size(); j++)
        {
            float animation = fmax(0, sinf(plume->getTransitionSpeed() * plume->getTransitionLifetime()[j]));
            float initial_radius = plume->get_r_0() / ratio;
            float half_size = 4.0f;
            float new_scaling = animation == 0 ? (half_size * 2.0f) : (half_size + (animation * half_size));
            float offset = (plume->get_z_0() / ratio) + terrain_display.uniform.transform.translation.z;
            vec3 new_translation = vec3(plume->getPosition().x / ratio - 25, plume->getPosition().y / ratio, offset + (animation * (fabs(offset) - 2)));
            float var = vcl::perlin(j, 2);

            quad_display->uniform.transform.rotation = rotation_from_axis_angle_mat3(camera->orientation.col(2), plume->getTransitionSpeed() * plume->getTransitionLifetime()[j] * var) * camera->orientation;
            quad_display->uniform.transform.translation = new_translation;
            quad_display->uniform.transform.scaling = initial_radius * new_scaling;
            quad_display->uniform.color = { 0.3f,0.3f,0.3f };
            quad_display->uniform.color_alpha = (0.8 + 0.3f * (2 * var - 1.0f)) * fmax(0.2f, animation);
            quad_display->shader = ShaderManager::getInstance()->getShader("mesh");
            quad_display->texture_id = smoke_texture;

            quad_display->draw(*camera);
        }
    }

    for (unsigned int j = 0; j < plume->free_spheres.size(); j++)
    {
        mat3 const R = rotation_from_axis_angle_mat3(plume->free_spheres[j].rotation_axis, plume->free_spheres[j].current_angle);
        float new_scaling = plume->free_spheres[j].r / ratio;
        //if (j==0) std::cout << new_scaling << std::endl;
        vec3 new_translation = vec3(plume->free_spheres[j].center.x / ratio - 25, plume->free_spheres[j].center.y / ratio, plume->free_spheres[j].center.z / ratio - 2);

        //sphere_display->uniform.transform.translation = new_translation;
        //sphere_display->uniform.transform.scaling = new_scaling;
        //sphere_display->uniform.transform.rotation = R;
        //sphere_display->uniform.color = { 1,1,1 };
        //sphere_display->shader = ShaderManager::getInstance()->getShader("mesh");

        float var = vcl::perlin(plume->free_spheres[j].id, 2);

        //quad_display->uniform.transform.rotation = rotation_from_axis_angle_mat3(camera->orientation.col(2), free_spheres[j].current_angle * dot(free_spheres[j].rotation_axis, camera->orientation.col(2)) * 1.5f *(1+0.3*var) + 2.2145*j*j) * camera->orientation;
        quad_display->uniform.transform.rotation = rotation_from_axis_angle_mat3(camera->orientation.col(2), plume->free_spheres[j].id * var) * camera->orientation;
        quad_display->uniform.transform.translation = new_translation;
        quad_display->uniform.transform.scaling = new_scaling * 1.3;
        quad_display->uniform.color_alpha = 0.8 + 0.3f * (2 * var - 1.0f);

        float l = (plume->free_spheres[j].lifetime / 120) + 0.3f;
        if (l > 1) l = 1;
        quad_display->uniform.color = { l,l,l };

        float end_fade = 1.0f;
        if (plume->free_spheres[j].lifetime >= plume->min_lifetime)
            end_fade -= (plume->free_spheres[j].lifetime - plume->min_lifetime) / (plume->max_lifetime - plume->min_lifetime);
        quad_display->uniform.color_alpha *= end_fade;
        quad_display->shader = ShaderManager::getInstance()->getShader("mesh");

        quad_display->draw(*camera);
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

        sphere_display->uniform.transform.translation = t;
        sphere_display->uniform.transform.scaling = r;
        sphere_display->uniform.transform.rotation = R;
        sphere_display->uniform.color = { disp_rho,disp_rho,disp_rho };
        sphere_display->shader = ShaderManager::getInstance()->getShader("mesh");
        sphere_display->draw(*camera);
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

        subspheres_display->uniform.transform.translation = t;
        subspheres_display->uniform.transform.scaling = r;
        subspheres_display->uniform.transform.rotation = R;
        subspheres_display->uniform.color = { disp_rho,disp_rho,disp_rho };
        subspheres_display->shader = ShaderManager::getInstance()->getShader("mesh");

        subspheres_display->draw(*camera);
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

            sphere_display->uniform.transform.translation = t;
            sphere_display->uniform.transform.scaling = r;
            sphere_display->uniform.color = { disp_rho,disp_rho,disp_rho };
            sphere_display->shader = ShaderManager::getInstance()->getShader("mesh");
            sphere_display->draw(*camera);
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
        vec3 new_translation = { plume->falling_spheres[j].center.x / ratio - 25, plume->falling_spheres[j].center.y / ratio, plume->falling_spheres[j].center.z / ratio - 2 };
        sphere_display->uniform.transform.translation = new_translation;
        sphere_display->uniform.transform.scaling = new_scaling;
        sphere_display->uniform.transform.rotation = mat3::identity();
        if (plume->falling_spheres[j].falling_under_atm_rho) sphere_display->uniform.color = { 1,0,0 };
        else sphere_display->uniform.color = { 1,1,1 };
        sphere_display->shader = ShaderManager::getInstance()->getShader("mesh");
        sphere_display->draw(*camera);
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
            sphere_display->uniform.transform.translation = new_translation;
            sphere_display->uniform.transform.scaling = new_scaling;
            sphere_display->uniform.transform.rotation = mat3::identity();
            if (plume->falling_spheres_buffers[k][j].falling_under_atm_rho) sphere_display->uniform.color = { 1,0,0 };
            else sphere_display->uniform.color = { 1,1,1 };
            sphere_display->shader = ShaderManager::getInstance()->getShader("mesh");
            sphere_display->draw(*camera);
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

        //terrain_display.norm_tex_id = add_normal_map(image_load_png("../assets/textures/Taal_Texture_normal_2024.png"));


        std::cout << "Pre-emptive terrain setup triggered" << "\n";
    }
}

//void scene_model::set_gui_profiler()
//{
//    Plume& plume = PlumeManager::getInstance()->getPlumes()[0];
//    ImGui::Begin("Profiler", NULL, ImGuiWindowFlags_AlwaysAutoResize);
//
//    std::string smoke_layers_count = "Smoke Layers: " + std::to_string(plume.smoke_layers.size());
//    std::string free_sphere_count = "Free Spheres: " + std::to_string(plume.free_spheres.size());
//    std::string falling_sphere_count = "Falling Spheres: " + std::to_string(plume.falling_spheres.size());
//    std::string stagnate_sphere_count = "Stagnate Spheres: " + std::to_string(plume.stagnate_spheres.size());
//    std::string subsphere_count = "Subspheres: " + std::to_string(plume.s2_spheres.size() + plume.s3_spheres.size());
//
//    ImGui::Text(smoke_layers_count.c_str());
//    ImGui::Text(free_sphere_count.c_str());
//    ImGui::Text(falling_sphere_count.c_str());
//    ImGui::Text(stagnate_sphere_count.c_str());
//    ImGui::Text(subsphere_count.c_str());
//
//    ImGui::End();
//}

void scene_model::stop()
{
    PlumeManager::getInstance()->stopSimulation();
    sim_input_screen->resetEruptOnPlay();
}

void scene_model::play()
{
    PlumeManager::getInstance()->playSimulation();
}

void scene_model::pause()
{
    PlumeManager::getInstance()->pauseSimulation();
}

void scene_model::keyboard_input(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    const bool key_escape = (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS);
    const bool key_space = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);

    SimulatorState state = PlumeManager::getInstance()->getState();
    if (key_escape)
    {
        stop();
    }

    if (key_space)
    {
         if (state == SimulatorState::Stopped || state == SimulatorState::Paused)
         {
             play();
         }
         else if (state == SimulatorState::Playing)
         {
             pause();
         }
    }
}
