#include "ProfilerScreen.hpp"
#include "singleton/PlumeManager.hpp"
#include "vcl/vcl.hpp"

ProfilerScreen::ProfilerScreen() : GUIScreen("Profiler")
{
    //pauseIcon = create_texture_gpu(image_load_png("../scenes/sources/smoke/images/pause_icon.png"));
    //playIcon = create_texture_gpu(image_load_png("../scenes/sources/smoke/images/play_icon.png"));
    //resetIcon = create_texture_gpu(image_load_png("../scenes/sources/smoke/images/undo_icon.png"));
}

ProfilerScreen::~ProfilerScreen()
{

}

void ProfilerScreen::drawGUI()
{
    //ImGui::Begin("Playback", &enabled, ImVec2(100, 73), -1.0f, ImGuiWindowFlags_NoResize);

    //// Start and stop animation
    //SimulatorState state = PlumeManager::getInstance()->getState();
    //if (state == SimulatorState::Stopped || state == SimulatorState::Paused)
    //{
    //    if (ImGui::ImageButton((ImTextureID)playIcon, ImVec2(32, 32)))
    //    {
    //        //play();
    //    }
    //}
    //else if (state == SimulatorState::Playing)
    //{
    //    if (ImGui::ImageButton((ImTextureID)pauseIcon, ImVec2(32, 32)))
    //    {
    //        //pause();
    //    }
    //}

    //if (state == SimulatorState::Playing || state == SimulatorState::Paused)
    //{
    //    ImGui::SameLine();
    //    if (ImGui::ImageButton((ImTextureID)resetIcon, ImVec2(32, 32)))
    //    {
    //        //stop();
    //    }
    //}
    //ImGui::End();

}
