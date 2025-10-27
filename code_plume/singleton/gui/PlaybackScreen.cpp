#include "PlaybackScreen.hpp"
#include "singleton/PlumeManager.hpp"
#include "singleton/TextureManager.hpp"

PlaybackScreen::PlaybackScreen() : GUIScreen("Playback")
{
    pauseIcon = TextureManager::getInstance()->getTexture("pause_icon");
    playIcon = TextureManager::getInstance()->getTexture("play_icon");
    resetIcon = TextureManager::getInstance()->getTexture("reset_icon");
}

PlaybackScreen::~PlaybackScreen()
{

}

void PlaybackScreen::drawGUI()
{
    ImGui::Begin("Playback", &enabled, ImVec2(100, 73), -1.0f, ImGuiWindowFlags_NoResize);

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
