#include "direction_tracker.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

void direction_tracker::loadData(std::string filePath)
{
    std::ifstream file;
    file.open(filePath);
    if (file.is_open())
    {
        for (std::string line; std::getline(file, line);)
        {
            std::istringstream ss(std::move(line));
            for (std::string value; std::getline(ss, value, ',');)
            {
                std::string name = std::move(value);
                danger_zone[name].arcStart = std::atof(std::move(value).c_str());
                danger_zone[name].arcEnd = std::atof(std::move(value).c_str());

                std::cout << "Location: " << name <<
                    ", Arc Start: " << danger_zone[name].arcStart <<
                    ", Arc End: " << danger_zone[name].arcEnd << "\n";
            }
        }
    }
    else std::cout << "ERROR: File with path " << filePath << " could not be opened.";

    file.close();
}

arc direction_tracker::get_arc(std::string name)
{
    return danger_zone[name];
}

void direction_tracker::show_gui()
{

}