#pragma once

#include "scenes/sources/smoke/smokeLayer.hpp"

#include <unordered_map>
#include <string>
#include <vector>

class direction_tracker
{
private:
	GLuint danger_zone_image;
	std::vector<float> arc_start;
	std::vector<float> arc_end;
	std::vector<std::string> location_names;

	const double pi = 3.14159;
	float cone_radius = 15.0f;
	float wind_angle = 0.0f;
	vcl::vec3 wind_vector = { 1.0f, 0.0f, 0.0f };

	std::vector<std::string> get_location_names(float angle);
	vcl::vec3 angle_to_vector(float angle) const;
	float vector_to_angle(vcl::vec3 vector) const;

public:
	void set_wind_direction(vcl::vec3 wind_vector);
	void load_data(std::string filePath);
    void show_gui();
};