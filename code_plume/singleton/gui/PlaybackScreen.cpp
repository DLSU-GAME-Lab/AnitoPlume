#include "PlaybackScreen.hpp"
#include "singleton/PlumeManager.hpp"
#include "singleton/TextureManager.hpp"

PlaybackScreen::PlaybackScreen() : GUIScreen("Playback")
{
    timer_scale = 1.0f;
    pauseIcon = TextureManager::getInstance()->getTexture("pause_icon");
    playIcon = TextureManager::getInstance()->getTexture("play_icon");
    resetIcon = TextureManager::getInstance()->getTexture("reset_icon");
}

PlaybackScreen::~PlaybackScreen()
{

}

void PlaybackScreen::drawGUI()
{
    ImGui::Begin("Playback", &enabled, ImVec2(100, 73), -1.0f, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::PushItemWidth(200);

    // Can set the speed of the animation
    float scale_min = 0.05f;
    float scale_max = 10.0f;
    if (ImGui::SliderScalar("Time scale", ImGuiDataType_Float, &timer_scale, &scale_min, &scale_max, "%.2f s"))
        PlumeManager::getInstance()->setTimerScale(timer_scale);

    ImGui::PopItemWidth();

    // Start and stop animation
    SimulatorState state = PlumeManager::getInstance()->getState();
    if (state == SimulatorState::Stopped || state == SimulatorState::Paused)
    {
        if (ImGui::ImageButton((ImTextureID)playIcon, ImVec2(32, 32)))
        {
            PlumeManager::getInstance()->playSimulation();
        }
    }
    else if (state == SimulatorState::Playing)
    {
        if (ImGui::ImageButton((ImTextureID)pauseIcon, ImVec2(32, 32)))
        {
            PlumeManager::getInstance()->pauseSimulation();
        }
    }

    if (state == SimulatorState::Playing || state == SimulatorState::Paused)
    {
        ImGui::SameLine();
        if (ImGui::ImageButton((ImTextureID)resetIcon, ImVec2(32, 32)))
        {
            PlumeManager::getInstance()->stopSimulation();
        }
    }
    ImGui::End();
}

float PlaybackScreen::getTimerScale() const
{
    return this->timer_scale;
}

void PlaybackScreen::setTimerScale(float timer_scale)
{
    this->timer_scale = timer_scale;
}
