#pragma once

#include "vcl/vcl.hpp"
#include <map>
#include <filesystem>

struct scene_structure
{
    vcl::camera_scene camera;
    vcl::camera_control_glfw camera_control;
    vcl::mesh_drawable frame_camera;
    vcl::mesh_drawable frame_worldspace;

    bool sky_enabled;
    vcl::vec4 clear_color;
    GLuint texture_white;
};

struct gui_structure
{
    GLFWwindow* window;
    std::string window_title;
    std::map<std::string, bool> enabled;

    bool show_frame_camera     = true;
    bool show_frame_worldspace = false;
    bool show_file_explorer = false;
};
