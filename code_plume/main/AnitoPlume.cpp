#include "AnitoPlume.hpp"

#include "singleton/GraphicsEngine.hpp"
#include "singleton/ShaderManager.hpp"
#include "singleton/MeshManager.hpp"
#include "singleton/GUIManager.hpp"
#include "singleton/CameraManager.hpp"

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

void windowSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    CameraManager::getInstance()->getCamera()->perspective.image_aspect = width / static_cast<float>(height);
}

void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    vcl::camera_scene* camera = CameraManager::getInstance()->getCamera();
    vcl::camera_control_glfw* controller = CameraManager::getInstance()->getController();
    controller->update_rotate(camera, window, float(xpos), float(ypos));
    scene_current.mouse_move(scene, window);
}
void mouseClickCallback(GLFWwindow* window, int button, int action, int mods)
{
    if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)) ImGui::SetWindowFocus(nullptr);

    vcl::camera_scene* camera = CameraManager::getInstance()->getCamera();
    vcl::camera_control_glfw* controller = CameraManager::getInstance()->getController();
    controller->update_mouse_click(camera, window, button, action, mods);
    scene_current.mouse_click(scene, window, button, action, mods);
}
void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    vcl::camera_scene* camera = CameraManager::getInstance()->getCamera();
    vcl::camera_control_glfw* controller = CameraManager::getInstance()->getController();
    CameraManager::getInstance()->getController()->update_mouse_scroll(camera, window, float(xoffset), float(yoffset));
    scene_current.mouse_scroll(scene, window, float(xoffset), float(yoffset));
}
void keyboardInputCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    vcl::camera_scene* camera = CameraManager::getInstance()->getCamera();
    vcl::camera_control_glfw* controller = CameraManager::getInstance()->getController();
    CameraManager::getInstance()->getController()->update_move(camera, window, key, scancode, action, mods);
    scene_current.keyboard_input(scene, window, key, scancode, action, mods);
}

AnitoPlume::AnitoPlume()
{
    // ************************************** //
    // Initialization and data setup
    // ************************************** //

    std::cout << "*** Init systems ***" << std::endl;
    GraphicsEngine::initialize();
    ShaderManager::initialize();
    MeshManager::initialize();
    std::cout << "\t [OK] systems Initialized" << std::endl;

    GraphicsEngine::getInstance()->createWindow("AnitoPlume");
    gui.window = GraphicsEngine::getInstance()->getWindow();

    CameraManager::initialize(GraphicsEngine::getInstance()->getWindow());

    GraphicsEngine::getInstance()->openglDebugInformation();

    std::cout << "*** Init imgui ***" << std::endl;
    GUIManager::initialize(GraphicsEngine::getInstance()->getWindow());
    std::cout << "\t [OK] imgui Initialized" << std::endl;

    // Set GLFW events listener
    glfwSetCursorPosCallback(GraphicsEngine::getInstance()->getWindow(), cursorPositionCallback);
    glfwSetMouseButtonCallback(GraphicsEngine::getInstance()->getWindow(), mouseClickCallback);
    glfwSetScrollCallback(GraphicsEngine::getInstance()->getWindow(), mouseScrollCallback);
    glfwSetKeyCallback(GraphicsEngine::getInstance()->getWindow(), keyboardInputCallback);
    glfwSetWindowSizeCallback(GraphicsEngine::getInstance()->getWindow(), windowSizeCallback);

    std::cout << "*** Setup Shader ***" << std::endl;
    ShaderManager::getInstance()->load("mesh", "mesh");
    ShaderManager::getInstance()->load("mesh_back_illumination", "mesh_bf");
    ShaderManager::getInstance()->load("wireframe", "wireframe", true);
    ShaderManager::getInstance()->load("wireframe_quads", "mewireframe_quads", true);
    ShaderManager::getInstance()->load("curve", "curve");
    ShaderManager::getInstance()->load("segment_immediate_mode", "segment_im");
    ShaderManager::getInstance()->load("normals", "normals", true);
    ShaderManager::getInstance()->load("skybox", "skybox");
    ShaderManager::getInstance()->load("mesh_mix", "mesh_mix");
    ShaderManager::getInstance()->load("sky_mesh", "sky_mesh");

    shaders["mesh"] = ShaderManager::getInstance()->getShader("mesh");
    shaders["mesh_bf"] = ShaderManager::getInstance()->getShader("mesh_bf");
    shaders["wireframe"] = ShaderManager::getInstance()->getShader("wireframe");
    shaders["wireframe_quads"] = ShaderManager::getInstance()->getShader("wireframe_quads");
    shaders["curve"] = ShaderManager::getInstance()->getShader("curve");
    shaders["segment_im"] = ShaderManager::getInstance()->getShader("segment_im");
    shaders["normals"] = ShaderManager::getInstance()->getShader("normals");
    shaders["skybox"] = ShaderManager::getInstance()->getShader("skybox");
    shaders["mesh_mix"] = ShaderManager::getInstance()->getShader("mesh_mix");
    shaders["sky_mesh"] = ShaderManager::getInstance()->getShader("sky_mesh");
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
    glfwGetWindowSize(GraphicsEngine::getInstance()->getWindow(), &width, &height);
    const float aspect_ratio = width / static_cast<float>(height);
    scene.camera.perspective = vcl::perspective_structure(40 * 3.14f / 180, aspect_ratio, 0.01f, 3000.0f);

    scene.sky_enabled = true;
    scene.clear_color = { 1.0f, 1.0f, 1.0f, 1.0f };
    const vcl::image_raw white{ 1,1,vcl::image_color_type::rgba,{255,255,255,255} };
    scene.texture_white = vcl::create_texture_gpu(white);
    //gui.enabled["Camera Settings"] = true;


    opengl_debug();
    std::cout << "*** Setup Data ***" << std::endl;
    scene_current.setup_data(shaders, scene, gui);
    std::cout << "\t [OK] Data setup" << std::endl;
    opengl_debug();
}

AnitoPlume::~AnitoPlume()
{
    CameraManager::destroy();
    GUIManager::destroy();
    MeshManager::destroy();
    ShaderManager::destroy();
    GraphicsEngine::destroy();
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

    while (!glfwWindowShouldClose(GraphicsEngine::getInstance()->getWindow()))
    {
        auto startTime = std::chrono::high_resolution_clock::now();

        this->processInput();

        this->update();

        this->render();

        if (fps_counter.update())
        {
            std::string window_title = GraphicsEngine::getInstance()->getWindowTitle();
            const std::string new_window_title = window_title + " (" + std::to_string(fps_counter.fps()) + " fps)";
            glfwSetWindowTitle(GraphicsEngine::getInstance()->getWindow(), new_window_title.c_str());
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
    GraphicsEngine::getInstance()->destroyWindow();
}

void AnitoPlume::processInput()
{
    glfwPollEvents();
    opengl_debug();
}

void AnitoPlume::update()
{
    CameraManager::getInstance()->getController()->update_timer();
}

void AnitoPlume::render()
{
    // Clear all color and zbuffer information before drawing on the screen
    GraphicsEngine::getInstance()->clearScreen();

    // Set a white image texture by default
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);

    // Create the basic gui structure with ImGui
    GUIManager::getInstance()->newFrame();

    // Perform computation and draw calls for each iteration loop
    scene_current.frame_draw(shaders, scene, gui);
    opengl_debug();

    // Render GUI and update window
    //scene.camera_control.update = !(ImGui::IsAnyWindowFocused());
    GUIManager::getInstance()->drawAllGUI(GraphicsEngine::getInstance()->getWindow());

    GraphicsEngine::getInstance()->swapBuffers();
}
