#include "MenuScreen.hpp"
#include "singleton/CameraManager.hpp"

MenuScreen::MenuScreen() : GUIScreen("MENU_SCREEN")
{

}

MenuScreen::~MenuScreen()
{

}

void MenuScreen::drawGUI()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            this->showFile();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            this->showEdit();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View"))
        {
            this->showView();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Window"))
        {
            this->showWindow();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {
            this->showHelp();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("About"))
        {
            this->showAbout();
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void MenuScreen::showFile()
{
    if (ImGui::MenuItem("Load Terrain..."))
    {
        //gui.show_file_explorer = true;
    }
    if (ImGui::MenuItem("Load Texture..."))
    {

    }

}

void MenuScreen::showEdit()
{

}

void MenuScreen::showView()
{
    vcl::camera_scene* camera = CameraManager::getInstance()->getCamera();
    ImGui::Checkbox("Enable Sky", &camera->sky_enabled);
    ImGui::SliderFloat("Gamma", &camera->gamma, 0.0f, 5.0f, "%.2f");
    ImGui::ColorEdit3("Viewport Clear Color", &camera->clear_color[0]);
    ImGui::Separator();
    ImGui::ColorEdit3("Fog Color", &camera->fog_color[0]);
    ImGui::InputFloat("Fog Density", &camera->fog_density, 0.0f, 0.1f, "%.4f");
    ImGui::SliderFloat("Fog Start Distance", &camera->fog_start, 0, 300, "%.2f");
    ImGui::SliderFloat("Fog Fade Height", &camera->fog_fade_height, -100, 300, "%.2f");
    ImGui::SliderFloat("Fog Max Height", &camera->fog_max_height, -100, 300, "%.2f");
}

void MenuScreen::showWindow()
{

    //for (auto it = gui.enabled.begin(); it != gui.enabled.end(); it++)
    //{
    //    if (ImGui::MenuItem(it->first.c_str()))
    //        gui.enabled[it->first] = !gui.enabled[it->first];
    //}

}

void MenuScreen::showHelp()
{

}

void MenuScreen::showAbout()
{

}
