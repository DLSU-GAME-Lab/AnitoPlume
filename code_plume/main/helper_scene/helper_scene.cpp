#include "helper_scene.hpp"
#include <GLFW/glfw3.h>


using namespace vcl;

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
            ImGui::Checkbox("Enable Sky", &scene.sky_enabled);
            ImGui::SliderFloat("Gamma", &scene.camera.gamma, 0.0f, 5.0f, "%.2f");
            ImGui::ColorEdit3("Viewport Clear Color", &scene.clear_color[0]);
            ImGui::Separator();
            ImGui::ColorEdit3("Fog Color", &scene.camera.fog_color[0]);
            ImGui::InputFloat("Fog Density", &scene.camera.fog_density, 0.0f, 0.1f, "%.4f");
            ImGui::SliderFloat("Fog Start Distance", &scene.camera.fog_start, 0, 300, "%.2f");
            ImGui::SliderFloat("Fog Fade Height", &scene.camera.fog_fade_height, -100, 300, "%.2f");
            ImGui::SliderFloat("Fog Max Height", &scene.camera.fog_max_height, -100, 300, "%.2f");
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
            scene.camera.translation = scene.camera.last_translation;
            scene.camera.apply_rotation_absolute(0.0f, 1.0f);
            break;
        }

        case view_mode::orbital:
        {
            scene.camera.set_scale(scene.camera_control.orbit_distance);
            scene.camera.last_translation = scene.camera.translation;
            scene.camera.translation = { 0.0f, 0.0f, -10.0f };
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

