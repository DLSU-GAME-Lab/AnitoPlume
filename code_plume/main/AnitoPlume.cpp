#include "AnitoPlume.hpp"

#include "singleton/GraphicsEngine.hpp"
#include "singleton/ShaderManager.hpp"
#include "singleton/MeshManager.hpp"
#include "singleton/TextureManager.hpp"
#include "singleton/GUIManager.hpp"
#include "singleton/CameraManager.hpp"
#include "singleton/EngineTime.hpp"
#include "singleton/PlumeManager.hpp"
#include "singleton/PlumeTracker.hpp"
// ************************************** //
// Global data declaration
// ************************************** //

// The graphical interface. Contains Window object and GUI related variables
gui_structure gui;

// Part specific data - you will specify this object in the corresponding exercise part
scene_model scene_current;

void windowSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    CameraManager::getInstance()->setImageAspect(width / static_cast<float>(height));
}

void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    CameraManager::getInstance()->update_rotate(window, float(xpos), float(ypos));
    //scene_current.mouse_move(window);
}
void mouseClickCallback(GLFWwindow* window, int button, int action, int mods)
{
    if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)) ImGui::SetWindowFocus(nullptr);

    CameraManager::getInstance()->update_mouse_click(window, button, action, mods);
    //scene_current.mouse_click(window, button, action, mods);
}
void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    CameraManager::getInstance()->update_mouse_scroll(window, float(xoffset), float(yoffset));
    //scene_current.mouse_scroll(window, float(xoffset), float(yoffset));
}
void keyboardInputCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    CameraManager::getInstance()->update_move(window, key, scancode, action, mods);
    scene_current.keyboard_input(window, key, scancode, action, mods);
}

AnitoPlume::AnitoPlume()
{
    // ************************************** //
    // Initialization and data setup
    // ************************************** //

    std::cout << "*** Init systems ***" << std::endl;
    EngineTime::initialize();
    GraphicsEngine::initialize();
    TextureManager::initialize();
    ShaderManager::initialize();
    MeshManager::initialize();
    PlumeManager::initialize();
    PlumeTracker::initialize();
    std::cout << "\t [OK] systems Initialized" << std::endl;

    GraphicsEngine::getInstance()->createWindow("AnitoPlume");
    gui.window = GraphicsEngine::getInstance()->getWindow();

    CameraManager::initialize(GraphicsEngine::getInstance()->getWindow());

    GraphicsEngine::getInstance()->openglDebugInformation();

    std::cout << "*** Setup GUI Resources ***" << std::endl;
    TextureManager::getInstance()->loadWhiteTexture();
    TextureManager::getInstance()->load("play_icon", "../assets/UI_Icons/play_icon.png");
    TextureManager::getInstance()->load("pause_icon", "../assets/UI_Icons/pause_icon.png");
    TextureManager::getInstance()->load("stop_icon", "../assets/UI_Icons/stop_icon.png");
    TextureManager::getInstance()->load("danger_zones", "../assets/images/danger_zones.png");
    std::cout << "\t [OK] GUI Resources loaded" << std::endl;

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
    std::cout << "\t [OK] Shader loaded" << std::endl;

    std::cout << "*** Setup Scene resources ***" << std::endl;
    scene_current.setup_resources();
    std::cout << "\t [OK] Scene resources loaded" << std::endl;

    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    //glFrontFace(GL_CCW);

    //scene.frame_camera = vcl::mesh_drawable(vcl::mesh_primitive_frame(0.15f, 0.05f, 0.15f, 0.3f));
    //scene.frame_camera.uniform.transform.scaling = 0.2f;
    //scene.frame_camera.shader = ShaderManager::getInstance()->getShader("mesh");

    //scene.frame_worldspace = vcl::mesh_drawable(vcl::mesh_primitive_frame(0.05f, 0.015f, 0.05f, 0.1f));
    //scene.frame_worldspace.shader = ShaderManager::getInstance()->getShader("mesh");

    opengl_debug();
    std::cout << "*** Setup Data ***" << std::endl;
    PlumeTracker::getInstance()->loadData("../assets/data/taal_danger_zones.csv");
    scene_current.setup_data();
    std::cout << "\t [OK] Data setup" << std::endl;
    opengl_debug();
}

AnitoPlume::~AnitoPlume()
{
    CameraManager::destroy();
    GUIManager::destroy();
    MeshManager::destroy();
    ShaderManager::destroy();
    TextureManager::destroy();
    GraphicsEngine::destroy();
    EngineTime::destroy();
    PlumeManager::destroy();
    PlumeTracker::destroy();
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
        EngineTime::getInstance()->logFrame();
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
        if (sleepTime > 0) std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(sleepTime)));

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
    scene_current.update();
}

void AnitoPlume::render()
{
    // Clear all color and zbuffer information before drawing on the screen
    GraphicsEngine::getInstance()->clearScreen();

    // Set a white image texture by default
    glBindTexture(GL_TEXTURE_2D, TextureManager::getInstance()->getTexture("white"));

    // Create the basic gui structure with ImGui
    GUIManager::getInstance()->newFrame();

    // Perform computation and draw calls for each iteration loop
    scene_current.frame_draw();
    opengl_debug();

    // Render GUI and update window
    //scene.camera_control.update = !(ImGui::IsAnyWindowFocused());
    GUIManager::getInstance()->drawAllGUI(GraphicsEngine::getInstance()->getWindow());

    GraphicsEngine::getInstance()->swapBuffers();
}
