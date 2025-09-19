#pragma once
#include "gui/GUIScreen.hpp"

#include <GLFW/glfw3.h>
#include <vector>
#include <unordered_map>

class GUIManager
{
public:
	typedef std::vector<GUIScreen*> GUIList;
	typedef std::unordered_map<std::string, GUIScreen*> GUITable;

public:
	static GUIManager* getInstance();
	static void initialize(GLFWwindow* window);
	static void destroy();

	void newFrame();
	//void processEvent();
	void drawAllGUI(GLFWwindow* window);

	GUIScreen* getGUIScreen(std::string name);
	bool getEnabled(std::string name);
	void setEnabled(std::string name, bool enabled);

private:
	GUIManager(GLFWwindow* window);
	~GUIManager();
	GUIManager(const GUIManager&) {};
	GUIManager operator=(const GUIManager&) {};
	static GUIManager* sharedInstance;


	GUIList guiList;
	GUITable guiTable;
};