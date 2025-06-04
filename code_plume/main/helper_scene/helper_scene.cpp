#include "helper_scene.hpp"
#include <GLFW/glfw3.h>


using namespace vcl;


GLFWwindow* create_window(const std::string& window_title)
{
    const int opengl_version_major = 3;
    const int opengl_version_minor = 3;
    const int window_width  = 1280;
    const int window_height = 1000;

    GLFWwindow* window = vcl::glfw_create_window(window_width, window_height, window_title, opengl_version_major, opengl_version_minor);
    return window;
}

void initialize_interface(gui_structure& gui)
{
    std::cout<<"*** Init GLFW ***"<<std::endl;
    vcl::glfw_init();
    std::cout<<"\t [OK] GLFW Initialized"<<std::endl;


    std::cout<<"*** Create window ***"<<std::endl;
    gui.window_title = "OpenGL Window";
    gui.window = create_window(gui.window_title);
    std::cout<<"\t [OK] Window Created"<<std::endl;

    std::cout<<"*** Init GLAD ***"<<std::endl;
    vcl::glad_init();
    std::cout<<"\t [OK] GLAD Initialized"<<std::endl;

    std::cout<<"*** OPENGL Information ***"<<std::endl;
    std::cout<<"======================================================="<<std::endl;
    vcl::opengl_debug_print_version();
    std::cout<<"======================================================="<<std::endl;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    std::cout<<"*** Init imgui ***"<<std::endl;
    imgui_init(gui.window);
    std::cout<<"\t [OK] imgui Initialized"<<std::endl;
}

void load_shaders(std::map<std::string,GLuint>& shaders)
{
    std::cout<<"*** Setup Shader ***"<<std::endl;

    shaders["mesh"] = create_shader_program("scenes/shared_assets/shaders/mesh/shader.vert.glsl","scenes/shared_assets/shaders/mesh/shader.frag.glsl");
    shaders["mesh_bf"] = create_shader_program("scenes/shared_assets/shaders/mesh_back_illumination/mesh.vert.glsl","scenes/shared_assets/shaders/mesh_back_illumination/mesh.frag.glsl");
    shaders["wireframe"] = create_shader_program("scenes/shared_assets/shaders/wireframe/shader.vert.glsl","scenes/shared_assets/shaders/wireframe/shader.geom.glsl","scenes/shared_assets/shaders/wireframe/shader.frag.glsl");
    shaders["wireframe_quads"] = create_shader_program("scenes/shared_assets/shaders/wireframe_quads/shader.vert.glsl","scenes/shared_assets/shaders/wireframe_quads/shader.geom.glsl","scenes/shared_assets/shaders/wireframe_quads/shader.frag.glsl");
    shaders["curve"] = create_shader_program("scenes/shared_assets/shaders/curve/shader.vert.glsl","scenes/shared_assets/shaders/curve/shader.frag.glsl");
    shaders["segment_im"] = create_shader_program("scenes/shared_assets/shaders/segment_immediate_mode/shader.vert.glsl","scenes/shared_assets/shaders/segment_immediate_mode/shader.frag.glsl");
    shaders["normals"] = create_shader_program("scenes/shared_assets/shaders/normals/shader.vert.glsl","scenes/shared_assets/shaders/normals/shader.geom.glsl","scenes/shared_assets/shaders/normals/shader.frag.glsl");
    shaders["skybox"] = create_shader_program("scenes/shared_assets/shaders/skybox/shader.vert.glsl","scenes/shared_assets/shaders/skybox/shader.frag.glsl");
    shaders["mesh_mix"] = create_shader_program("scenes/shared_assets/shaders/mesh_mix/shader.vert.glsl", "scenes/shared_assets/shaders/mesh_mix/shader.frag.glsl");
    shaders["sky_mesh"] = create_shader_program("scenes/shared_assets/shaders/sky_mesh/shader.vert.glsl", "scenes/shared_assets/shaders/sky_mesh/shader.frag.glsl");
    std::cout<<"\t [OK] Shader loaded"<<std::endl;
}

void setup_scene(scene_structure &scene, gui_structure& gui, const std::map<std::string,GLuint>& shaders)
{

    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    //glFrontFace(GL_CCW);

    scene.frame_camera = mesh_drawable(mesh_primitive_frame(0.15f, 0.05f, 0.15f, 0.3f));
    scene.frame_camera.uniform.transform.scaling = 0.2f;
    scene.frame_camera.shader = shaders.at("mesh");

    scene.frame_worldspace = mesh_drawable(mesh_primitive_frame(0.05f, 0.015f, 0.05f, 0.1f));
    scene.frame_worldspace.shader = shaders.at("mesh");

    int width=0, height=0;
    glfwGetWindowSize(gui.window, &width, &height);
    const float aspect_ratio = width/static_cast<float>(height);

    scene.camera.perspective = perspective_structure( 40*3.14f/180, aspect_ratio, 0.01f, 3000.0f);

    scene.clear_color = { 0.7f, 0.9f, 1.0f, 1.0f };
    const image_raw white{1,1,image_color_type::rgba,{255,255,255,255}};
    scene.texture_white = create_texture_gpu(white);

    gui.enabled["Camera Settings"] = true;
}

void clear_screen(scene_structure& scene)
{
    glClearColor(scene.clear_color[0], scene.clear_color[1], scene.clear_color[2], scene.clear_color[3]);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
}

void update_fps_title(GLFWwindow* window, const std::string& title, glfw_fps_counter& fps_counter)
{
    if ( fps_counter.update() )
    {
        const std::string new_window_title = title+" ("+std::to_string(fps_counter.fps())+" fps)";
        glfwSetWindowTitle(window, new_window_title.c_str());
        fps_counter.reset();
    }
}

void gui_start_basic_structure(gui_structure& gui, scene_structure& scene)
{
    //ImGui::Begin("Frame Draw", NULL, ImGuiWindowFlags_AlwaysAutoResize);
    //ImGui::Checkbox("Frame camera", &gui.show_frame_camera);
    //ImGui::Checkbox("Frame worldspace", &gui.show_frame_worldspace);

    //if(gui.show_frame_camera)
    //{
    //    scene.frame_camera.uniform.transform.translation = -scene.camera.translation;
    //    draw(scene.frame_camera, scene.camera);//  scene.frame_camera.draw(shaders.at("mesh"),scene.camera);
    //}
    //if(gui.show_frame_worldspace)
    //    draw(scene.frame_worldspace, scene.camera);

    //ImGui::End();
}

void gui_main_menu_bar(gui_structure& gui, scene_structure& scene)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Load Terrain..."))
            {
                gui.show_file_explorer = true;
            }
            if (ImGui::MenuItem("Load Texture..."))
            {

            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View"))
        {
            ImGui::ColorPicker4("Clear Color", &scene.clear_color[0]);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Window"))
        {
            for (auto it = gui.enabled.begin(); it != gui.enabled.end(); it++)
            {
                if (ImGui::MenuItem(it->first.c_str()))
                    gui.enabled[it->first] = !gui.enabled[it->first];
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("About"))
        {

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void gui_camera_settings(gui_structure& gui, scene_structure& scene)
{
    ImGui::Begin("Camera Settings", &gui.enabled["Camera Settings"], ImGuiWindowFlags_AlwaysAutoResize);
    
    ImGui::SliderScalar(
        "Speed",
        ImGuiDataType_Float,
        &scene.camera_control.speed,
        &scene.camera_control.speed_min,
        &scene.camera_control.speed_max,
        "%.2f");

    int index = (int)scene.camera.mode;
    static const char* preview_val[] { "Aerial", "Orbital", /*"Bystander"*/ };

    bool update_mode = false;
    for (int i = 0; i < IM_ARRAYSIZE(preview_val); i++)
    {
        if (i > 0) ImGui::SameLine();
        if (ImGui::RadioButton(preview_val[i], index == i) && index != i)
        {
            update_mode = true;
            index = i;
        }
    }

    //if (ImGui::Combo("View Mode", &index, preview_val, IM_ARRAYSIZE(preview_val))) update_mode = true;

    if (update_mode)
    {
        if (scene.camera.mode == view_mode::orbital)
            scene.camera.apply_last_translation();

        scene.camera.mode = (view_mode)index;
        std::cout << "View mode updated to " << preview_val[index] << "\n";

        switch (scene.camera.mode)
        {
        case view_mode::aerial:
        {
            scene.camera.set_scale(scene.camera.distance);
            scene.camera.apply_rotation_absolute(0.0f, 1.0f);
            break;
        }

        case view_mode::orbital:
        {
            scene.camera.set_scale(scene.camera_control.orbit_distance);
            scene.camera.reset_translation();
            scene.camera.apply_rotation_absolute(0.0f, 1.0f);
            break;
        }

        case view_mode::bystander:
        {
            scene.camera.set_scale(scene.camera.distance);
            scene.camera.apply_rotation_absolute(0.0f, 1.0f);
            break;
        }

        default:
            break;
        }
    }

    switch (scene.camera.mode)
    {
    case view_mode::aerial:
    case view_mode::bystander:
    {
        float pos[] = { -scene.camera.translation.x, -scene.camera.translation.y, -scene.camera.translation.z };
        if (ImGui::InputFloat3("Position", pos, "%.2f"))
        {
            scene.camera.limit_translation(vec3{ -pos[0], -pos[1], -pos[2] });
        }
        break;
    }
    
    case view_mode::orbital:
    {

        if (ImGui::SliderScalar(
            "Distance",
            ImGuiDataType_Float,
            &scene.camera_control.orbit_distance,
            &scene.camera_control.orbit_min,
            &scene.camera_control.orbit_max,
            "%.4f"))
        {
            scene.camera.set_scale(scene.camera_control.orbit_distance);
        }

        break;
    }
    
    default:
        break;
    }

    ImGui::End();

}

