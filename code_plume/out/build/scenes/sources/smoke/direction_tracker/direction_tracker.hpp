#pragma once

#include <unordered_map>
#include <string>
#include <vector>

class direction_tracker
{
private:
	std::vector<float> arc_start;
	std::vector<float> arc_end;
	std::vector<std::string> location_names;

	float wind_direction = 0.0f;

	std::vector<std::string> get_location_names(float angle);

public:

	void set_wind_direction(float angle);
	void load_data(std::string filePath);
    void show_gui();
};