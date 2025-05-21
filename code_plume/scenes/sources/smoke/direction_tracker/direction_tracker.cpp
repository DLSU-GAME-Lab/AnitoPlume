#include "direction_tracker.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

void direction_tracker::load_data(std::string filePath)
{
    std::string texture_path = "../scenes/sources/smoke/images/danger_zones.png";
    danger_zone_image = vcl::create_texture_gpu(vcl::image_load_png(texture_path));

    std::ifstream file;
    file.open(filePath);
    if (file.is_open())
    {
        for (std::string line; std::getline(file, line);)
        {
            std::istringstream ss(std::move(line));
            std::vector<std::string> cell;
            for (std::string value; std::getline(ss, value, ',');)
                cell.push_back(std::move(value));

            this->location_names.push_back(cell[0]);
            this->arc_start.push_back(std::atof(cell[1].c_str()));
            this->arc_end.push_back(std::atof(cell[2].c_str()));
        }
    }
    else std::cout << "ERROR: File with path " << filePath << " could not be opened.";

    file.close();
}

std::vector<std::string> direction_tracker::get_location_names(float angle)
{
    std::vector<std::string> affected_locs;
    std::vector<int> arc_start_indices;
    std::vector<int> arc_end_indices;

    for (int i = 0; i < this->arc_start.size(); i++)
    {
        if (this->arc_start[i] <= angle)
            arc_start_indices.push_back(i);
    }
    
    for (int arcIndex : arc_start_indices)
    {
        if (this->arc_end[arcIndex] >= angle)
            arc_end_indices.push_back(arcIndex);
    }

    for (int locIndex : arc_end_indices)
        affected_locs.push_back(this->location_names[locIndex]);

    return affected_locs;
}

void direction_tracker::set_wind_direction(vcl::vec3 wind_vector)
{
    this->wind_vector = wind_vector;
    float radians = atan2f(wind_vector.y, wind_vector.x);
    this->wind_angle = (180 * radians / 3.14159);
}

void direction_tracker::show_gui()
{
    ImGui::Begin("Direction Tracker", NULL, ImGuiWindowFlags_AlwaysAutoResize);

    const float image_size = 256.0f;
    const float half_size = image_size / 2.0f;
    ImGui::Image((ImTextureID)danger_zone_image, { image_size, image_size });
    std::vector<std::string> affected_locs = get_location_names(this->wind_angle);

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 win_pos = ImGui::GetWindowPos();
    win_pos.x += 5; win_pos.y += 25;
    ImVec2 start = ImVec2(win_pos.x + half_size, win_pos.y + half_size);
    ImVec2 end = ImVec2(start.x + (half_size * wind_vector.x), -start.y - (half_size * wind_vector.y));
    draw_list->AddLine(start, end, IM_COL32(240, 0, 20, 255), 2.0f);

    ImGui::SameLine();
    ImGui::BeginChild("Affected Areas", {200.0f, image_size}, true);

    ImGui::SetWindowFontScale(1.5f);
    ImGui::TextColored({0.9f, 0.0f, 0.1f, 1.0f}, "Affected Areas:");
    for (int i = 0; i < affected_locs.size(); i++)
        ImGui::Text(affected_locs[i].c_str());
    ImGui::SetWindowFontScale(1.0f);

    ImGui::EndChild();
    ImGui::End();
}