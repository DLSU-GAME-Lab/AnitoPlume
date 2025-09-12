#include "AnitoPlume.hpp"

// ************************************** //
// Global data declaration
// ************************************** //

// Storage for shaders indexed by their names
std::map<std::string, GLuint> shaders;

// General shared elements of the scene such as camera and its controler, visual elements, etc
scene_structure scene;

// The graphical interface. Contains Window object and GUI related variables
gui_structure gui;

// Part specific data - you will specify this object in the corresponding exercise part
scene_model scene_current;

void window_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    scene.camera.perspective.image_aspect = width / static_cast<float>(height);;
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    scene.camera_control.update_rotate(scene.camera, window, float(xpos), float(ypos));
    scene_current.mouse_move(scene, window);
}
void mouse_click_callback(GLFWwindow* window, int button, int action, int mods)
{
    if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)) ImGui::SetWindowFocus(nullptr);

    scene.camera_control.update_mouse_click(scene.camera, window, button, action, mods);
    scene_current.mouse_click(scene, window, button, action, mods);
}
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    scene.camera_control.update_mouse_scroll(scene.camera, window, float(xoffset), float(yoffset));
    scene_current.mouse_scroll(scene, window, float(xoffset), float(yoffset));
}
void keyboard_input_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    scene.camera_control.update_move(scene_current.terrain_struct, scene.camera, window, key, scancode, action, mods);
    scene_current.keyboard_input(scene, window, key, scancode, action, mods);
}

AnitoPlume::AnitoPlume()
{
    // ************************************** //
    // Initialization and data setup
    // ************************************** //

    // Initialize external libraries and window
    std::cout << "*** Init GLFW ***" << std::endl;
    vcl::glfw_init();
    std::cout << "\t [OK] GLFW Initialized" << std::endl;

    std::cout << "*** Create window ***" << std::endl;
    gui.window_title = "AnitoPlume";
    const int opengl_version_major = 3;
    const int opengl_version_minor = 3;
    const int window_width = 1920;
    const int window_height = 1080;

    gui.window = vcl::glfw_create_window(
        window_width,
        window_height,
        gui.window_title,
        opengl_version_major,
        opengl_version_minor);
    std::cout << "\t [OK] Window Created" << std::endl;

    std::cout << "*** Init GLAD ***" << std::endl;
    vcl::glad_init();
    std::cout << "\t [OK] GLAD Initialized" << std::endl;

    std::cout << "*** OPENGL Information ***" << std::endl;
    std::cout << "=======================================================" << std::endl;
    vcl::opengl_debug_print_version();
    std::cout << "=======================================================" << std::endl;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    std::cout << "*** Init imgui ***" << std::endl;
    vcl::imgui_init(gui.window);
    std::cout << "\t [OK] imgui Initialized" << std::endl;

    // Set GLFW events listener
    glfwSetCursorPosCallback(gui.window, cursor_position_callback);
    glfwSetMouseButtonCallback(gui.window, mouse_click_callback);
    glfwSetScrollCallback(gui.window, mouse_scroll_callback);
    glfwSetKeyCallback(gui.window, keyboard_input_callback);
    glfwSetWindowSizeCallback(gui.window, window_size_callback);

    std::cout << "*** Setup Shader ***" << std::endl;
    shaders["mesh"] = vcl::create_shader_program("scenes/shared_assets/shaders/mesh/shader.vert.glsl", "scenes/shared_assets/shaders/mesh/shader.frag.glsl");
    shaders["mesh_bf"] = vcl::create_shader_program("scenes/shared_assets/shaders/mesh_back_illumination/mesh.vert.glsl", "scenes/shared_assets/shaders/mesh_back_illumination/mesh.frag.glsl");
    shaders["wireframe"] = vcl::create_shader_program("scenes/shared_assets/shaders/wireframe/shader.vert.glsl", "scenes/shared_assets/shaders/wireframe/shader.geom.glsl", "scenes/shared_assets/shaders/wireframe/shader.frag.glsl");
    shaders["wireframe_quads"] = vcl::create_shader_program("scenes/shared_assets/shaders/wireframe_quads/shader.vert.glsl", "scenes/shared_assets/shaders/wireframe_quads/shader.geom.glsl", "scenes/shared_assets/shaders/wireframe_quads/shader.frag.glsl");
    shaders["curve"] = vcl::create_shader_program("scenes/shared_assets/shaders/curve/shader.vert.glsl", "scenes/shared_assets/shaders/curve/shader.frag.glsl");
    shaders["segment_im"] = vcl::create_shader_program("scenes/shared_assets/shaders/segment_immediate_mode/shader.vert.glsl", "scenes/shared_assets/shaders/segment_immediate_mode/shader.frag.glsl");
    shaders["normals"] = vcl::create_shader_program("scenes/shared_assets/shaders/normals/shader.vert.glsl", "scenes/shared_assets/shaders/normals/shader.geom.glsl", "scenes/shared_assets/shaders/normals/shader.frag.glsl");
    shaders["skybox"] = vcl::create_shader_program("scenes/shared_assets/shaders/skybox/shader.vert.glsl", "scenes/shared_assets/shaders/skybox/shader.frag.glsl");
    shaders["mesh_mix"] = vcl::create_shader_program("scenes/shared_assets/shaders/mesh_mix/shader.vert.glsl", "scenes/shared_assets/shaders/mesh_mix/shader.frag.glsl");
    shaders["sky_mesh"] = vcl::create_shader_program("scenes/shared_assets/shaders/sky_mesh/shader.vert.glsl", "scenes/shared_assets/shaders/sky_mesh/shader.frag.glsl");
    std::cout << "\t [OK] Shader loaded" << std::endl;


    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    //glFrontFace(GL_CCW);

    scene.frame_camera = vcl::mesh_drawable(vcl::mesh_primitive_frame(0.15f, 0.05f, 0.15f, 0.3f));
    scene.frame_camera.uniform.transform.scaling = 0.2f;
    scene.frame_camera.shader = shaders.at("mesh");

    scene.frame_worldspace = vcl::mesh_drawable(vcl::mesh_primitive_frame(0.05f, 0.015f, 0.05f, 0.1f));
    scene.frame_worldspace.shader = shaders.at("mesh");

    int width = 0, height = 0;
    glfwGetWindowSize(gui.window, &width, &height);
    const float aspect_ratio = width / static_cast<float>(height);
    scene.camera.perspective = vcl::perspective_structure(40 * 3.14f / 180, aspect_ratio, 0.01f, 3000.0f);

    scene.sky_enabled = true;
    scene.clear_color = { 1.0f, 1.0f, 1.0f, 1.0f };
    const vcl::image_raw white{ 1,1,vcl::image_color_type::rgba,{255,255,255,255} };
    scene.texture_white = vcl::create_texture_gpu(white);
    gui.enabled["Camera Settings"] = true;


    opengl_debug();
    std::cout << "*** Setup Data ***" << std::endl;
    scene_current.setup_data(shaders, scene, gui);
    std::cout << "\t [OK] Data setup" << std::endl;
    opengl_debug();
}

AnitoPlume::~AnitoPlume()
{

}

void AnitoPlume::run()
{
    // ************************************** //
    // Animation loop
    // ************************************** //

    std::cout << "*** Start GLFW animation loop ***" << std::endl;

    const int FRAME_LIMIT = 120;
    const float FRAME_TIME = 1000.0f / FRAME_LIMIT;

    vcl::glfw_fps_counter fps_counter;

    while (!glfwWindowShouldClose(gui.window))
    {
        auto startTime = std::chrono::high_resolution_clock::now();

        this->processInput();

        this->update();

        this->render();

        if (fps_counter.update())
        {
            const std::string new_window_title = gui.window_title + " (" + std::to_string(fps_counter.fps()) + " fps)";
            glfwSetWindowTitle(gui.window, new_window_title.c_str());
            fps_counter.reset();
        }

        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float, std::milli> elapsedTime = endTime - startTime;
        float sleepTime = FRAME_TIME - elapsedTime.count();

        if (sleepTime > 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(sleepTime)));
        }
    }
    std::cout << "*** Stop GLFW loop ***" << std::endl;

    // Cleanup ImGui and GLFW
    vcl::imgui_cleanup();

    glfwDestroyWindow(gui.window);
    glfwTerminate();
}

void AnitoPlume::processInput()
{
    glfwPollEvents();
    opengl_debug();
}

void AnitoPlume::update()
{
    scene.camera_control.update_timer();
}

void AnitoPlume::render()
{
    // Clear all color and zbuffer information before drawing on the screen
    glClearColor(scene.clear_color[0], scene.clear_color[1], scene.clear_color[2], scene.clear_color[3]);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    opengl_debug();

    // Set a white image texture by default
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);

    // Create the basic gui structure with ImGui
    vcl::imgui_create_frame();

    gui_main_menu_bar(gui, scene);
    gui_camera_settings(gui, scene);

    // Perform computation and draw calls for each iteration loop
    scene_current.frame_draw(shaders, scene, gui);

    opengl_debug();

    // Render GUI and update window
    scene.camera_control.update = !(ImGui::IsAnyWindowFocused());
    vcl::imgui_render_frame(gui.window);

    glfwSwapBuffers(gui.window);
}
