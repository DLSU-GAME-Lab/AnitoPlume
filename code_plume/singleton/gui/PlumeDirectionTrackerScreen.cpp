#include "PlumeDirectionTrackerScreen.hpp"
#include "singleton/TextureManager.hpp"
#include "singleton/PlumeTracker.hpp"
#include "singleton/PlumeManager.hpp"

PlumeDirectionTrackerScreen::PlumeDirectionTrackerScreen() : GUIScreen("Plume Direction Tracker")
{
    dangerZoneImage = TextureManager::getInstance()->getTexture("minimap");
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
    
    float windAngle = PlumeManager::getInstance()->getAverageWindAngle();
    float coneRadius = PlumeTracker::getInstance()->getConeRadius() / ratio;
    int maxAltitude = PlumeManager::getInstance()->getMaxAlt();

    if (layeredViewEnabled)
    {
        for (int i = 0; i < PlumeTracker::getInstance()->getDataCount(); i++)
        {
            if (PlumeTracker::getInstance()->getPositions(i).empty()) continue;
            std::vector<vcl::vec3> positions = PlumeTracker::getInstance()->getPositions(i);
            std::vector<float> radii = PlumeTracker::getInstance()->getRadii(i);
            for (int i = 0; i < positions.size(); i++)
            {
                vcl::vec2 pos = vcl::vec2(positions[i].x, positions[i].y);
                pos /= ratio;
                pos += vcl::vec2(xOffset, yOffset);
                pos.y *= -1;

                float mapRadius = (imgSize / 2) - 32;

                if (pos.x > -mapRadius && pos.x < mapRadius &&
                    pos.y > -mapRadius && pos.y < mapRadius)
                {
                    ImVec2 center = ImVec2(start.x + pos.x, start.y + pos.y);
                    float mult = vcl::mag(vcl::vec2(positions[i].x - positions[0].x, positions[i].y - positions[0].y));
					mult = vcl::clamp(mult / 20000.0f, 0.0f, 1.0f);
                    float radius = ((radii[i] + (radii[i] * mult)) / ratio) + 1;

                    const int col = 100;
                    //int col = 64 + (192 * mult);
                    //if (col > 255) col = 255;
                    drawList->AddCircleFilled(center, radius, IM_COL32(col, col, col, 200));
                }
            }
        }
    }

    if (windVectorsEnabled)
    {
        int windSize = PlumeManager::getInstance()->getWinds().size();
        ImVec2 startVec = { 0, 0 };
        ImVec2 endVec = { 0, 0 };
        for (int i = 0; i < windSize; i++)
        {
            vcl::vec3 wind = PlumeManager::getInstance()->getWinds()[i].wind_vector / 10;
            if (i == 0) startVec = start;
            else startVec = endVec;
            endVec = ImVec2(startVec.x + wind.x, startVec.y - wind.y);
            drawList->AddLine(startVec, endVec, IM_COL32(250, 250 - (i * 50), i * 10, 255), lineThickness + i);
        }

        if (startVec.x != endVec.x || startVec.y != endVec.y)
        {
			float angle = PlumeManager::getInstance()->getWinds()[windSize - 1].angle;
			angle *= (180 / PI);
            const float len1 = 6.0f;
            const float len2 = 12.0f;
            vcl::vec3 vec1 = vcl::angle_to_vector(angle - 90.0f);
            vcl::vec3 vec2 = vcl::angle_to_vector(angle + 90.0f);
            vcl::vec3 vec3 = vcl::angle_to_vector(angle);
            ImVec2 tip1 = ImVec2(endVec.x + (vec1.x * len1), endVec.y + (-vec1.y * len1));
            ImVec2 tip2 = ImVec2(endVec.x + (vec2.x * len1), endVec.y + (-vec2.y * len1));
            ImVec2 tip3 = ImVec2(endVec.x + (vec3.x * len2), endVec.y + (-vec3.y * len2));
			drawList->AddTriangleFilled(tip1, tip2, tip3, red);
        }
    }

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

    ImGui::SameLine();
    showAffectedAreas();

    ImGui::Checkbox("Display layered view", &layeredViewEnabled);
    ImGui::SameLine();
    ImGui::Spacing();
    ImGui::SameLine();
    ImGui::Checkbox("Display wind vectors", &windVectorsEnabled);
    //ImGui::SameLine();
    //ImGui::Spacing();
    //ImGui::SameLine();
    //ImGui::Checkbox("Display wind cone", &windConeEnabled);
	//ImGui::SliderFloat("Ratio", &ratio, 100.0f, 600.0f, "%.1f");

    ImGui::End();
}

void PlumeDirectionTrackerScreen::showAffectedAreas()
{
    ImGui::BeginChild("Affected Areas", { 200.0f, imgSize }, true);
    float coneRadius = PlumeTracker::getInstance()->getConeRadius() / ratio;
    std::vector<std::string> affectedAreas = PlumeTracker::getInstance()->getIntersectingLocations(coneRadius);
    ImGui::SetWindowFontScale(1.5f);
	ImGui::Indent(34.0f);
    ImGui::TextColored({ 0.9f, 0.0f, 0.1f, 1.0f }, "Affected Areas");
	ImGui::Unindent(34.0f);
    ImGui::Separator();

    if (affectedAreas.empty()) ImGui::Text("None");
    else
    {
        for (int i = 0; i < affectedAreas.size(); i++)
            ImGui::Text(affectedAreas[i].c_str());
    }
    
    ImGui::SetWindowFontScale(1.0f);
    ImGui::EndChild();
}
