#pragma once
#include "vcl/interaction/interaction.hpp"

class CameraManager
{
private:
	CameraManager(GLFWwindow* window);
	~CameraManager();
	CameraManager(const CameraManager&) {};
	CameraManager operator=(const CameraManager&) {};
	static CameraManager* sharedInstance;

public:
	static CameraManager* getInstance();
	static void initialize(GLFWwindow* window);
	static void destroy();

	void setImageAspect(float imageAspect);
	void update_rotate(GLFWwindow* window, float xpos, float ypos);
	void update_mouse_click(GLFWwindow* window, int button, int action, int mods);
	void update_mouse_scroll(GLFWwindow* window, float xoffset, float yoffset);
	void update_move(GLFWwindow* window, int key, int scancode, int action, int mods);

	vcl::camera_scene* getCamera();
	vcl::camera_control_glfw* getController();

private:
	vcl::camera_scene camera;
	vcl::camera_control_glfw controller;
};