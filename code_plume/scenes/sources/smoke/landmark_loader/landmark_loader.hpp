#pragma once
#include "scenes/base/base.hpp"
#include <filesystem>
#include <thread>
struct landmark_loader
{
	static const int landmark_count = 11;
	//mesh mesh_terrain;
	vcl::mesh_drawable* landmark;
	vcl::mesh_drawable landmark_display[landmark_count];

	GLuint shader;
	GLuint texture_id[landmark_count];

	float landmark_min_dist = 80;
	float landmark_max_dist = 30;

	void setup_landmarks();
	void draw();
};