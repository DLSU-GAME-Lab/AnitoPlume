#pragma once

#include "../camera/camera.hpp"
#include "../time_slider/time_slider.hpp"
#include "scenes/sources/smoke/terrain_structure.hpp"
#include <GLFW/glfw3.h>


namespace vcl
{
    
class camera_control_glfw
{
public:

    void update_mouse_move(camera_scene& camera, GLFWwindow* window, float x1, float y1);
    void update_mouse_click(camera_scene& camera, GLFWwindow* window,  int button, int action, int mods);
    void update_mouse_scroll(camera_scene& camera, GLFWwindow* window, float xoffset, float yoffset);

    void update_timer();
    void update_rotate(camera_scene& camera, GLFWwindow* window, float x1, float y1);
    void update_move(terrain_structure& terrain_struct, camera_scene& camera, GLFWwindow* window, int key, int scancode, int action, int mods);

    bool update = true;

    float speed = 20.0f;
    float speed_min = 1.0f;
    float speed_max = 100.0f;
    float scroll_speed = 20.0f;
    
    float orbit_distance = 3.0f;
    float orbit_min = 1.0f;
    float orbit_max = 10.0f;

    float eye_line = 2.0f;

private:

    /** previous mouse position (x-coordinate) */
    float x0 = 0.0f;
    /** previous mouse position (y-coordinate) */
    float y0 = 0.0f;

    timer_basic timer;
    float dt = 0.0f;

};

}
