#pragma once

#include "vcl/vcl.hpp"
#include <unordered_map>
#include <string>
#include <vector>

class direction_tracker
{
private:
	float max_altitude;
	float altitude_step;
	int steps;
	GLuint danger_zone_image;
	std::vector<float> arc_start;
	std::vector<float> arc_end;
	std::vector<std::string> location_names;
	std::vector<vcl::vec3> positions;
	std::vector<float> radii;
	bool layered_view_enabled = true;
	bool predicton_enabled = false;
	bool enabled = true;

	const double pi = 3.14159;
	const float ratio = 144.0f;
	float cone_radius = 0.0f;
	float wind_angle = 0.0f;
	vcl::vec3 wind_vector = { 0.0f, 0.0f, 0.0f };

	std::vector<std::string> get_location_names(float angle);
	void show_affected_areas(float image_size);

public:
	void initialize(float max_altitude, int steps);
	void set_altitude_step(float altitude_step);
	void load_data(std::string filePath);

	void set_wind_direction(vcl::vec3 wind_vector);
	void set_plume_positions(unsigned int index, vcl::vec3 position, float radius);
	void reset_plume_positions();

    void show_gui(bool* show);
};