#include "skybox.hpp"

vcl::skybox::skybox()
{
	vertices.push_back(vec3(-1.0f, 1.0f, -1.0f)); // Top-left
	vertices.push_back(vec3( 1.0f, 1.0f, -1.0f)); // Top-right
	vertices.push_back(vec3( 1.0f,-1.0f, -1.0f)); // Bottom-right
	vertices.push_back(vec3( 1.0f,-1.0f, -1.0f)); // Bottom-right
	vertices.push_back(vec3(-1.0f,-1.0f, -1.0f)); // Bottom-left
	vertices.push_back(vec3(-1.0f, 1.0f, -1.0f)); // Top-left

    connectivity.push_back({ 1,2,6 });
    connectivity.push_back({ 6,5,1 });
    connectivity.push_back({ 0,4,7 });
    connectivity.push_back({ 7,3,0 });
    connectivity.push_back({ 4,5,6 });
    connectivity.push_back({ 6,7,4 });
    connectivity.push_back({ 0,3,2 });
    connectivity.push_back({ 2,1,0 });
    connectivity.push_back({ 0,1,5 });
    connectivity.push_back({ 5,4,0 });
    connectivity.push_back({ 3,7,6 });
    connectivity.push_back({ 6,2,3 });
}
