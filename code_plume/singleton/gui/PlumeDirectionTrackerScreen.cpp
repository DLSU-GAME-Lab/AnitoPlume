#include "PlumeDirectionTrackerScreen.hpp"
#include "singleton/PlumeTracker.hpp"
#include "singleton/PlumeManager.hpp"

PlumeDirectionTrackerScreen::PlumeDirectionTrackerScreen() : GUIScreen("Plume Direction Tracker")
{
    dangerZoneImage = vcl::create_texture_gpu(vcl::image_load_png("../scenes/sources/smoke/images/danger_zones.png"));
}

PlumeDirectionTrackerScreen::~PlumeDirectionTrackerScreen()
{

}

void PlumeDirectionTrackerScreen::drawGUI()
{
    ImGui::Begin("Plume Direction Tracker", &enabled, ImGuiWindowFlags_AlwaysAutoResize);
    
    ImGui::Image((ImTextureID)dangerZoneImage, { imgSize, imgSize });

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 winPos = ImGui::GetWindowPos();
    winPos.x += 8; winPos.y += 27;
    ImVec2 start = ImVec2(winPos.x + halfSize, winPos.y + halfSize);
    
    float windAngle = PlumeTracker::getInstance()->getWindDirectionAngle();
    float coneRadius = PlumeTracker::getInstance()->getConeRadius() / ratio;
    int maxAltitude = PlumeManager::getInstance()->getMaxAlt();

    if (windConeEnabled && coneRadius != 0)
    {
        vcl::vec3 wind1 = vcl::angle_to_vector(windAngle + coneRadius);
        vcl::vec3 wind2 = vcl::angle_to_vector(windAngle - coneRadius);

        ImVec2 end1 = ImVec2(start.x + (wind1.x * lineLen), start.y + (-wind1.y * lineLen));
        ImVec2 end2 = ImVec2(start.x + (wind2.x * lineLen), start.y + (-wind2.y * lineLen));

        drawList->AddLine(start, end1, red, lineThickness);
        drawList->AddLine(start, end2, red, lineThickness);
        drawList->AddTriangleFilled(start, end1, end2, alphaRed);
    }

    if (layeredViewEnabled)
    {
        for (int i = 0; i < PlumeTracker::getInstance()->getDataCount(); i++)
        {
            std::vector<vcl::vec3> positions = PlumeTracker::getInstance()->getPositions(i);
            std::vector<float> radii = PlumeTracker::getInstance()->getRadii(i);
            for (int i = 0; i < positions.size(); i++)
            {
                vcl::vec2 pos = vcl::vec2(positions[i].x, positions[i].y);
                pos /= ratio;
                pos.x += xOffset;
                pos.y *= -1;

                float map_r = (imgSize / 2) - 32;

                if (pos.x > -map_r && pos.x < map_r &&
                    pos.y > -map_r && pos.y < map_r)
                {
                    ImVec2 center = ImVec2(start.x + pos.x, start.y + pos.y);
                    float radius = radii[i] / ratio;

                    if (int(positions[i].z) >= int(maxAltitude) + 1)
                        radius *= ((i * altitudeStep) / maxAltitude) + 0.1f;

                    int col = 64 + (192 * (i / (steps * 0.75f)));
                    if (col > 255) col = 255;
                    drawList->AddCircleFilled(center, radius, IM_COL32(col, col, col, 200));
                }
            }
        }
    }

    ImGui::SameLine();
    showAffectedAreas();

    ImGui::Checkbox("Display layered view", &layeredViewEnabled);
    ImGui::SameLine();
    ImGui::Spacing();
    ImGui::SameLine();
    ImGui::Checkbox("Display wind cone", &windConeEnabled);

    ImGui::End();
}

void PlumeDirectionTrackerScreen::showAffectedAreas()
{
    ImGui::BeginChild("Affected Areas", { 200.0f, imgSize }, true);
    std::vector<std::string> affectedAreas = PlumeTracker::getInstance()->getIntersectingLocations();
    ImGui::SetWindowFontScale(1.5f);
    ImGui::TextColored({ 0.9f, 0.0f, 0.1f, 1.0f }, "Affected Areas:");
    ImGui::Text("None");
    //for (int i = 0; i < affectedAreas.size(); i++)
    //    ImGui::Text(affectedAreas[i].c_str());

    ImGui::SetWindowFontScale(1.0f);
    ImGui::EndChild();
}
