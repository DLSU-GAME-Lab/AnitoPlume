#pragma once

#include "scenes/sources/smoke/smokeLayer.hpp"
#include <filesystem>
#include <thread>

struct terrain_loader
{
	//mesh mesh_terrain;
	vcl::mesh_drawable terrain;
	GLuint mesh_shader;
	GLuint current_tex_id;
	GLuint current_norm_id;
	GLuint texture_id[5];
	GLuint normal_id[5];
	bool new_terrain_loaded = false;
	bool enabled = true;

	const int size = 32;
	std::string current_terrain = "";
	std::string current_texture = "";

	char ter_input[32] = "Taal-Spherical-2_0.obj";
	char tex_input[32] = "Taal_Texture_2023.png";

	int current_tex = 0;

	void load_all_textures();

	void show_gui(bool* show);
	void load_terrain(std::string terrain_filename, std::string texture_filename);
	void load_terrain_async(std::string terrain_filename, std::string texture_filename);
};