#include "skybox.hpp"

vcl::skybox::skybox()
{
	vertices.push_back(vec3(-1.0f, 1.0f, -1.0f)); // Top-left
	vertices.push_back(vec3( 1.0f, 1.0f, -1.0f)); // Top-right
	vertices.push_back(vec3( 1.0f,-1.0f, -1.0f)); // Bottom-right
	vertices.push_back(vec3( 1.0f,-1.0f, -1.0f)); // Bottom-right
	vertices.push_back(vec3(-1.0f,-1.0f, -1.0f)); // Bottom-left
	vertices.push_back(vec3(-1.0f, 1.0f, -1.0f)); // Top-left
}
