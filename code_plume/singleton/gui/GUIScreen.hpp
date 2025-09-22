#pragma once

#include "third_party/imgui/imgui.h"
#include "third_party/imgui/imgui_impl_glfw.h"
#include "third_party/imgui/imgui_impl_opengl3.h"

#include <string>

class GUIManager;

class GUIScreen
{
protected:
	GUIScreen(std::string name);
	~GUIScreen();

	std::string getName();
	virtual void drawGUI() = 0;

	std::string name;
	bool enabled = true;

	friend class GUIManager;
};