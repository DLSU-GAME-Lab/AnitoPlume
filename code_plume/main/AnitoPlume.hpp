#pragma once

// Include VCL library
#include "vcl/vcl.hpp"

// Include common part for exercises
#include "main/helper_scene/helper_scene.hpp"

// Include exercises
#include "scenes/scenes.hpp"

// ************************************** //
// GLFW event listeners
// ************************************** //

void windowSizeCallback(GLFWwindow* window, int width, int height);
void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
void mouseClickCallback(GLFWwindow* window, int button, int action, int mods);
void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void keyboardInputCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

class AnitoPlume
{
private:
	

public:
	AnitoPlume();
	~AnitoPlume();

	void run();

private:
	void processInput();
	void update();
	void render();

};