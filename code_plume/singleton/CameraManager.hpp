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

	vcl::camera_scene* getCamera();
	vcl::camera_control_glfw* getController();

private:
	vcl::camera_scene camera;
	vcl::camera_control_glfw controller;
};