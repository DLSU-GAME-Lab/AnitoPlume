#include "direction_tracker.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

void direction_tracker::initialize(float max_altitude, int steps)
{
    this->max_altitude = max_altitude;
    this->steps = steps;
}

void direction_tracker::set_altitude_step(float altitude_step)
{
    this->altitude_step = altitude_step;
}

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
    float low_angle = angle - cone_radius;
    float hi_angle = angle + cone_radius;

    for (int i = 0; i < this->arc_start.size(); i++)
    {
        float arc_start = this->arc_start[i];
        float arc_end = this->arc_end[i];

        if (arc_start > arc_end)
        {
            if (angle <= 180) arc_start -=360;
            else if (angle > 180) arc_end += 360;
        }

        if ((arc_start <= low_angle || arc_start <= hi_angle) &&
            (arc_end >= low_angle || arc_end >= hi_angle))
            affected_locs.push_back(this->location_names[i]);
    }

    return affected_locs;
}

vcl::vec3 direction_tracker::angle_to_vector(float angle) const
{
    double radians = (angle * pi) / 180;
    return vcl::vec3(cos(radians), sin(radians), 0);
}

float direction_tracker::vector_to_angle(vcl::vec3 vector) const
{
    double radians = atan2(wind_vector.y, wind_vector.x);
    if (radians < 0) radians += (2 * pi);
    return radians * (180.0 / pi);
}

void direction_tracker::show_affected_areas(float image_size)
{
    ImGui::BeginChild("Affected Areas", { 200.0f, image_size }, true);
    std::vector<std::string> affected_locs = get_location_names(this->wind_angle);
    ImGui::SetWindowFontScale(1.5f);
    ImGui::TextColored({ 0.9f, 0.0f, 0.1f, 1.0f }, "Affected Areas:");
    for (int i = 0; i < affected_locs.size(); i++)
        ImGui::Text(affected_locs[i].c_str());
    ImGui::SetWindowFontScale(1.0f);
    ImGui::EndChild();
}

void direction_tracker::set_plume_positions(unsigned int index, vcl::vec3 position, float radius)
{
    if (index == this->positions.size())
    {
        this->positions.push_back(position);
        this->radii.push_back(radius);
        this->cone_radius = radii[radii.size() - 1] / ratio;
    }
    else if (index < this->positions.size())
    {
        this->positions[index] = position;
        this->radii[index] = radius;
        this->cone_radius = radii[radii.size() - 1] / ratio;
    }
}

void direction_tracker::reset_plume_positions()
{
    this->positions.clear();
    this->radii.clear();
    this->cone_radius = 0.0f;
}

void direction_tracker::set_wind_direction(vcl::vec3 wind_vector)
{
    this->wind_vector = wind_vector;
    this->wind_angle = vector_to_angle(wind_vector);
}

void direction_tracker::show_gui(bool* show)
{
    ImGui::Begin("Plume Direction Tracker", &enabled, ImGuiWindowFlags_AlwaysAutoResize);
    *show = enabled;

    const float image_size = 256.0f;
    const float half_size = image_size / 2.0f;
    const float line_len = 108.0f;
    ImGui::Image((ImTextureID)danger_zone_image, { image_size, image_size });

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 win_pos = ImGui::GetWindowPos();
    win_pos.x += 8; win_pos.y += 27;
    ImVec2 start = ImVec2(win_pos.x + half_size, win_pos.y + half_size);

    const float line_thk = 2.0f;
    ImU32 red = IM_COL32(240, 0, 40, 255);
    ImU32 alpha_red = IM_COL32(240, 0, 40, 100);

    if (predicton_enabled && cone_radius != 0)
    {
        float angle = vector_to_angle(wind_vector);
        vcl::vec3 wind1 = angle_to_vector(angle + cone_radius);
        vcl::vec3 wind2 = angle_to_vector(angle - cone_radius);

        ImVec2 end1 = ImVec2(start.x + (wind1.x * line_len), start.y + (-wind1.y * line_len));
        ImVec2 end2 = ImVec2(start.x + (wind2.x * line_len), start.y + (-wind2.y * line_len));

        draw_list->AddLine(start, end1, red, line_thk);
        draw_list->AddLine(start, end2, red, line_thk);
        draw_list->AddTriangleFilled(start, end1, end2, alpha_red);
    }

    if (layered_view_enabled)
    {
        float x_offset = -18;
        for (int i = 0; i < positions.size(); i++)
        {
            vcl::vec2 pos = vcl::vec2(positions[i].x, positions[i].y);
            pos /= ratio;
            pos.x += x_offset;
            pos.y *= -1;

            float map_r = (image_size / 2) - 32;

            if (pos.x > -map_r && pos.x < map_r &&
                pos.y > -map_r && pos.y < map_r)
            {
                ImVec2 center = ImVec2(start.x + pos.x, start.y + pos.y);
                float radius = radii[i] / ratio;

                if (int(positions[i].z) >= int(max_altitude) + 1)
                    radius *= ((i * altitude_step) / max_altitude) + 0.1f;

                int col = 64 + (192 * (i / (steps * 0.75f)));
                if (col > 255) col = 255;
                draw_list->AddCircleFilled(center, radius, IM_COL32(col, col, col, 200));
            }
        }
    }

    ImGui::SameLine();
    show_affected_areas(image_size);

    ImGui::Checkbox("Display layered view", &layered_view_enabled);
    ImGui::SameLine();
    ImGui::Spacing();
    ImGui::SameLine();
    ImGui::Checkbox("Display predicted direction", &predicton_enabled);

    ImGui::End();
}
