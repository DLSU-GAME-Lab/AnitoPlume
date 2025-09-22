#include "CameraManager.hpp"

CameraManager* CameraManager::sharedInstance = nullptr;

CameraManager::CameraManager(GLFWwindow* window)
{
	int width = 0, height = 0;
	glfwGetWindowSize(window, &width, &height);
	const float aspect_ratio = width / static_cast<float>(height);
	camera.perspective = vcl::perspective_structure(40 * 3.14f / 180, aspect_ratio, 0.01f, 3000.0f);

	camera.set_scale(controller.orbit_distance);
	camera.translation = { 0.0f, 0.0f, -10.0f };
	camera.apply_rotation_absolute(0.0f, 1.0f);
	camera.last_translation = { 0.0f, 100.0f, -10.0f };
}

CameraManager::~CameraManager()
{

}

CameraManager* CameraManager::getInstance()
{
	return sharedInstance;
}
void CameraManager::initialize(GLFWwindow* window)
{
	sharedInstance = new CameraManager(window);
}

void CameraManager::destroy()
{
	delete sharedInstance;
}

vcl::camera_scene* CameraManager::getCamera()
{
	return &camera;
}

vcl::camera_control_glfw* CameraManager::getController()
{
	return &controller;
}
