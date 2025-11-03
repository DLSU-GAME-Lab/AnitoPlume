#include "TerrainScreen.hpp"
#include "singleton/TextureManager.hpp"
#include <iostream>

TerrainScreen::TerrainScreen() : GUIScreen("Terrain")
{

}

TerrainScreen::~TerrainScreen()
{

}

void TerrainScreen::initialize()
{
    currentTexID = TextureManager::getInstance()->getTexture("Taal_Texture_2023");
    currentNormID = TextureManager::getInstance()->getTexture("Taal_Normal_2023");
}

void TerrainScreen::drawGUI()
{
    ImGui::Begin("Terrain", &enabled, ImGuiWindowFlags_AlwaysAutoResize);

    const char* years[]{ "2023", "2021", "2019", "2016" };
    if (ImGui::Combo("Year", &currentTex, years, IM_ARRAYSIZE(years)))
    {
        std::string year = years[currentTex];
        std::string textureName = "Taal_Texture_" + year;
        std::string normalName = "Taal_Normal_" + year;
        currentTexID = TextureManager::getInstance()->getTexture(textureName);
        currentNormID = TextureManager::getInstance()->getTexture(normalName);
    }

    //ImGui::InputText("Terrain", ter_input, size, ImGuiInputTextFlags_AutoSelectAll);
    //ImGui::InputText("Texture", tex_input, size, ImGuiInputTextFlags_AutoSelectAll);
    //if (ImGui::Button("Load Terrain"))
    //{
    //    std::string terrain_filename = ter_input;
    //    std::string texture_filename = tex_input;
    //}

    ImGui::End();
}

unsigned int TerrainScreen::getCurrentTex() const
{
    return currentTexID;
}

unsigned int TerrainScreen::getCurrentNormTex() const
{
    return currentNormID;
}
