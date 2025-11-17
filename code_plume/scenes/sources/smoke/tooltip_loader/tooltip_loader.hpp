#pragma once
#include "scenes/base/base.hpp"
#include <filesystem>
#include <thread>
struct tooltip_loader
{
	//mesh mesh_terrain;
	vcl::mesh_drawable tooltip;
	GLuint mesh_shader;
	GLuint current_tex_id;
	GLuint texture_id[5];
	bool new_tooltip_loaded = false;

	const int size = 32;
	std::string current_tooltip = "Tooltip-Balantoc.obj";
	std::string current_texture = "Tooltip-Balantoc.png";

	char tip_input[32] = "Tooltip-Balantoc.obj";
	char tex_input[32] = "Tooltip-Balantoc.png";

	int current_tex = 0;

	void load_all_textures();
	void load_tooltip(std::string terrain_filename, std::string texture_filename, bool isTrans = false);
};