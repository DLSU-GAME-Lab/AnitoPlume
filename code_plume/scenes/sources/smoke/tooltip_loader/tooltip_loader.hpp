#pragma once
#include "scenes/base/base.hpp"
#include <filesystem>
#include <thread>
struct tooltip_loader
{
	vcl::mesh_drawable* tooltip;
	vcl::mesh_drawable tooltip_display[4];

	GLuint shader;
	GLuint texture_id[5];
	float tooltip_dist = 100.0f;

	void setup_tooltips();
	void draw();
};