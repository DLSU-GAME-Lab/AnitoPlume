#pragma once
#include "scenes/base/base.hpp"
#include <filesystem>
#include <thread>
struct tooltip_loader
{
	vcl::mesh_drawable* tooltip;
	std::vector<vcl::mesh_drawable> tooltip_display;

	GLuint shader;
	GLuint texture_id[5];
	float tooltip_dist = 100.0f;
	vcl::vec3 last_cam_pos = {};

	void setup_tooltips();
	void draw();
};