#include "direction_tracker.hpp"
#include "scenes/sources/smoke/smokeLayer.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

void direction_tracker::load_data(std::string filePath)
{
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

            int index = this->location_names.size() - 1;
            std::cout << "Location: " << this->location_names[index] <<
                ", Arc Start: " << this->arc_start[index] <<
                ", Arc End: " << this->arc_end[index] << "\n";
        }
    }
    else std::cout << "ERROR: File with path " << filePath << " could not be opened.";

    file.close();
}

std::vector<std::string> direction_tracker::get_location_names(float angle)
{
    std::vector<std::string> affectedLocs;
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
        affectedLocs.push_back(this->location_names[locIndex]);

    return affectedLocs;
}

void direction_tracker::set_wind_direction(float angle)
{
    this->wind_direction = angle;
}

void direction_tracker::show_gui()
{
    ImGui::Begin("Direction Tracker");

    std::vector<std::string> affectedLocs = get_location_names(this->wind_direction);
    std::string affectedText;

    for (int i = 0; i < affectedLocs.size(); i++)
    {
        affectedText += affectedLocs[i];
        if (i < affectedLocs.size() - 1)
            affectedText += ", ";
    }

    ImGui::Text(affectedText.c_str());
    ImGui::End();
}