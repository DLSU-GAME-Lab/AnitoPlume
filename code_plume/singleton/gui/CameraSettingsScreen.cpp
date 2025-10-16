#include "CameraSettingsScreen.hpp"
#include "singleton/CameraManager.hpp"

using namespace vcl;

CameraSettingsScreen::CameraSettingsScreen() : GUIScreen("Camera Settings")
{

}

CameraSettingsScreen::~CameraSettingsScreen()
{

}

void CameraSettingsScreen::drawGUI()
{
    ImGui::Begin("Camera Settings", &enabled, ImGuiWindowFlags_AlwaysAutoResize);

    camera_scene* camera = CameraManager::getInstance()->getCamera();
    camera_control_glfw* controller = CameraManager::getInstance()->getController();

    ImGui::SliderScalar(
        "Speed",
        ImGuiDataType_Float,
        &controller->speed,
        &controller->speed_min,
        &controller->speed_max,
        "%.2f");

    int index = (int)camera->mode;
    static const char* preview_val[]{ "Aerial", "Orbital", /*"Bystander"*/ };

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
        if (camera->mode == view_mode::orbital)
            camera->apply_last_translation();

        camera->mode = (view_mode)index;
        std::cout << "View mode updated to " << preview_val[index] << "\n";

        switch (camera->mode)
        {
        case view_mode::aerial:
        {
            camera->set_scale(camera->distance);
            camera->translation = camera->last_translation;
            camera->apply_rotation_absolute(0.0f, 1.0f);
            break;
        }

        case view_mode::orbital:
        {
            camera->set_scale(controller->orbit_distance);
            camera->last_translation = camera->translation;
            camera->translation = { 0.0f, 0.0f, -10.0f };
            camera->apply_rotation_absolute(0.0f, 1.0f);
            break;
        }

        case view_mode::bystander:
        {
            camera->set_scale(camera->distance);
            camera->apply_rotation_absolute(0.0f, 1.0f);
            break;
        }

        default:
            break;
        }
    }

    switch (camera->mode)
    {
    case view_mode::aerial:
    case view_mode::bystander:
    {
        float pos[] = { -camera->translation.x, -camera->translation.y, -camera->translation.z };
        if (ImGui::InputFloat3("Position", pos, "%.2f"))
        {
            camera->limit_translation(vec3{ -pos[0], -pos[1], -pos[2] });
        }
        break;
    }

    case view_mode::orbital:
    {

        if (ImGui::SliderScalar(
            "Distance",
            ImGuiDataType_Float,
            &controller->orbit_distance,
            &controller->orbit_min,
            &controller->orbit_max,
            "%.4f"))
        {
            camera->set_scale(controller->orbit_distance);
        }

        break;
    }

    default:
        break;
    }

    ImGui::End();
}
