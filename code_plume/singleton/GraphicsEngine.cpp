#include "GraphicsEngine.hpp"

GraphicsEngine* GraphicsEngine::sharedInstance = nullptr;

GraphicsEngine::GraphicsEngine()
{
	this->window = nullptr;
	this->window_title = {};

	std::cout << "*** Init GLFW ***" << std::endl;
	vcl::glfw_init();
	std::cout << "\t [OK] GLFW Initialized" << std::endl;

}

GraphicsEngine::~GraphicsEngine()
{

}

GraphicsEngine* GraphicsEngine::getInstance()
{
	return sharedInstance;
}

void GraphicsEngine::initialize()
{
	sharedInstance = new GraphicsEngine();
}

void GraphicsEngine::destroy()
{
	delete sharedInstance;
}

void GraphicsEngine::createWindow(std::string window_title)
{
	std::cout << "*** Create window ***" << std::endl;
	this->window_title = window_title;

	this->window = vcl::glfw_create_window(
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		this->window_title,
		OPENGL_VERSION_MAJOR,
		OPENGL_VERSION_MINOR);
	std::cout << "\t [OK] Window Created" << std::endl;

	std::cout << "*** Init GLAD ***" << std::endl;
	vcl::glad_init();
	std::cout << "\t [OK] GLAD Initialized" << std::endl;
}

void GraphicsEngine::openglDebugInformation()
{
	std::cout << "*** OPENGL Information ***" << std::endl;
	std::cout << "=======================================================" << std::endl;
	vcl::opengl_debug_print_version();
	std::cout << "=======================================================" << std::endl;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void GraphicsEngine::clearScreen()
{
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	opengl_debug();
}

GLFWwindow* GraphicsEngine::getWindow()
{
	return this->window;
}

std::string GraphicsEngine::getWindowTitle()
{
	return this->window_title;
}

void GraphicsEngine::setWindowTitle(std::string title)
{
	this->window_title = title;
}
