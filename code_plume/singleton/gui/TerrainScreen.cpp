#include "TerrainScreen.hpp"

TerrainScreen::TerrainScreen() : GUIScreen("Terrain")
{

}

TerrainScreen::~TerrainScreen()
{

}

void TerrainScreen::drawGUI()
{
    ImGui::Begin("Terrain", &enabled, ImGuiWindowFlags_AlwaysAutoResize);

    static const char* labels[]{ "2023", "2021", "2019", "2016" };
    if (ImGui::Combo("Year", &currentTex, labels, IM_ARRAYSIZE(labels)))
    {
        currentTexID = textureID[currentTex];
        currentNormID = normalID[currentTex];
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
