#pragma once

#include "third_party/imgui/imgui.h"
#include "third_party/imgui/imgui_impl_glfw.h"
#include "third_party/imgui/imgui_impl_opengl3.h"

#include <string>

class GUIScreen
{
public:
	std::string getName() const;
	bool getEnabled() const;
	void setEnabled(bool enabled);

protected:
	GUIScreen(std::string name);
	~GUIScreen();

	virtual void drawGUI() = 0;

	std::string name;
	bool enabled = true;

	friend class GUIManager;
};