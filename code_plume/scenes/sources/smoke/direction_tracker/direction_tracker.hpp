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
	std::vector<vcl::vec2> positions;
	std::vector<float> radii;
	bool layered_view_enabled = true;
	bool enabled = true;

	const double pi = 3.14159;
	const float ratio = 100.0f;
	float radius_mult_at_max = 1.2f;
	float cone_radius = 5.0f;
	float wind_angle = 0.0f;
	vcl::vec3 wind_vector = { 0.0f, 0.0f, 0.0f };

	std::vector<std::string> get_location_names(float angle);
	vcl::vec3 angle_to_vector(float angle) const;
	float vector_to_angle(vcl::vec3 vector) const;

	void show_affected_areas(float image_size);

public:
	void set_plume_positions(unsigned int index, vcl::vec3 position, float radius);
	void reset_plume_positions();

	void set_wind_direction(vcl::vec3 wind_vector);
	void load_data(std::string filePath);
    void show_gui(bool* show);
};