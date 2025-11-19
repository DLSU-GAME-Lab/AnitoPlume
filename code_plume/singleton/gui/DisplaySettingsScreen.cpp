#include "DisplaySettingsScreen.hpp"
#include "singleton/TextureManager.hpp"
#include "singleton/PlumeManager.hpp"
#include <iostream>

DisplaySettingsScreen::DisplaySettingsScreen() : GUIScreen("Display Settings")
{

}

DisplaySettingsScreen::~DisplaySettingsScreen()
{

}

void DisplaySettingsScreen::initialize()
{
    currentTexID = TextureManager::getInstance()->getTexture("Taal_Texture_2023");
    currentNormID = TextureManager::getInstance()->getTexture("Taal_Normal_2023");
}

void DisplaySettingsScreen::drawGUI()
{
    ImGui::Begin("Display Settings", &enabled, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::PushItemWidth(150);

    const char* years[]{ "2023", "2021", "2019", "2016" };
    if (ImGui::Combo("Terrain by Year", &currentTex, years, IM_ARRAYSIZE(years)))
    {
        std::string year = years[currentTex];
        std::string textureName = "Taal_Texture_" + year;
        std::string normalName = "Taal_Normal_" + year;
        currentTexID = TextureManager::getInstance()->getTexture(textureName);
        currentNormID = TextureManager::getInstance()->getTexture(normalName);
    }

    //// Parameters
    //std::vector<Plume>& plumes = PlumeManager::getInstance()->getPlumes();
    //unsigned int spheres_min = 0, spheres_max = 500;
    //static unsigned int subspheres_number = 0, subsubspheres_number = 0;
    //if (ImGui::SliderScalar("Number of subspheres", ImGuiDataType_S32, &subspheres_number, &spheres_min, &spheres_max))
    //    for (int i = 0; i < plumes.size(); i++) plumes[i].subspheres_number = subspheres_number;

    //if (ImGui::SliderScalar("Number of subsubspheres", ImGuiDataType_S32, &subsubspheres_number, &spheres_min, &spheres_max))
    //    for (int i = 0; i < plumes.size(); i++) plumes[i].subsubspheres_number = subsubspheres_number;

    ImGui::PopItemWidth();

    ImGui::Checkbox("Display Tooltips", &display_tooltips);
    ImGui::Checkbox("Display Landmarks", &display_landmarks);
    ImGui::Checkbox("Display billboards", &display_billboards);
    ImGui::Checkbox("Display free spheres", &display_free_spheres);
    ImGui::Checkbox("Display spheres with subspheres", &display_spheres_with_subspheres);
    ImGui::Checkbox("Display torus layers", &display_smoke_layers);

    ImGui::End();
}

unsigned int DisplaySettingsScreen::getCurrentTex() const
{
    return currentTexID;
}

unsigned int DisplaySettingsScreen::getCurrentNormTex() const
{
    return currentNormID;
}

bool DisplaySettingsScreen::getDisplaySmokeLayers() const
{
    return this->display_smoke_layers;
}

void DisplaySettingsScreen::setDisplaySmokeLayers(bool display)
{
    this->display_smoke_layers = display;
}

bool DisplaySettingsScreen::getDisplayFreeSpheres() const
{
    return this->display_free_spheres;
}

void DisplaySettingsScreen::setDisplayFreeSpheres(bool display)
{
    this->display_free_spheres = display;
}

bool DisplaySettingsScreen::getDisplaySubspheres() const
{
    return this->display_subspheres;
}

void DisplaySettingsScreen::setDisplaySubspheres(bool display)
{
    this->display_subspheres = display;
}

bool DisplaySettingsScreen::getDisplaySpheresWithSubspheres() const
{
    return this->display_spheres_with_subspheres;
}

void DisplaySettingsScreen::setDisplaySpheresWithSubspheres(bool display)
{
    this->display_spheres_with_subspheres = display;
}

bool DisplaySettingsScreen::getDisplayBillboards() const
{
    return this->display_billboards;
}

void DisplaySettingsScreen::setDisplayBillboards(bool display)
{
    this->display_billboards = display;
}

bool DisplaySettingsScreen::getDisplayTooltips() const
{
    return this->display_tooltips;
}

void DisplaySettingsScreen::setDisplayTooltips(bool display)
{
    this->display_tooltips;
}

bool DisplaySettingsScreen::getDisplayLandmarks() const
{
    return this->display_landmarks;
}

void DisplaySettingsScreen::setDisplayLandmarks(bool display)
{
    this->display_landmarks = display;
}
