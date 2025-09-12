#pragma once

// Include VCL library
#include "vcl/vcl.hpp"

class GraphicsEngine
{
private:
	const int OPENGL_VERSION_MAJOR = 3;
	const int OPENGL_VERSION_MINOR = 3;
	const int WINDOW_WIDTH = 1920;
	const int WINDOW_HEIGHT = 1080;

	GLFWwindow* window;
	std::string window_title;

public:
	static GraphicsEngine* getInstance();
	void initialize();
	void destroy();

	void createWindow();
	void clearScreen();

	GLFWwindow* getWindow();
	std::string getWindowTitle();

	void setWindowTitle(std::string title);

private:
	GraphicsEngine();
	~GraphicsEngine();
	GraphicsEngine(const GraphicsEngine&) {};
	GraphicsEngine operator=(const GraphicsEngine&) {};
	static GraphicsEngine* sharedInstance;
};