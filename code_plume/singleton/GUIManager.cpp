#include "GUIManager.hpp"
#include"gui/MenuScreen.hpp"
#include "gui/CameraSettingsScreen.hpp"
#include "gui/SimulatorInputScreen.hpp"
#include "gui/PlaybackScreen.hpp"
#include "gui/ProfilerScreen.hpp"

void GUIManager::newFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void GUIManager::drawAllGUI(GLFWwindow* window)
{
    for (int i = 0; i < this->guiList.size(); i++)
    {
        if (this->guiList[i]->enabled)
            this->guiList[i]->drawGUI();
    }

    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

GUIScreen* GUIManager::getGUIScreen(std::string name)
{
    return this->guiTable[name];
}

std::vector<GUIScreen*>& GUIManager::getGUIScreens()
{
    return this->guiList;
}

bool GUIManager::getEnabled(std::string name)
{
    GUIScreen* gui = this->guiTable[name];
    return gui ? gui->enabled : false;
}

void GUIManager::setEnabled(std::string name, bool enabled)
{
    GUIScreen* gui = this->guiTable[name];
    if (gui) gui->enabled = enabled;
}

GUIManager* GUIManager::sharedInstance = nullptr;

GUIManager::GUIManager(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 150");
    ImGui::StyleColorsDark();

    MenuScreen* menuScreen = new MenuScreen();
    this->guiTable[menuScreen->name] = menuScreen;
    this->guiList.push_back(menuScreen);

    CameraSettingsScreen* camScreen = new CameraSettingsScreen();
    this->guiTable[camScreen->name] = camScreen;
    this->guiList.push_back(camScreen);

    SimulatorInputScreen* simInputScreen = new SimulatorInputScreen();
    this->guiTable[simInputScreen->name] = simInputScreen;
    this->guiList.push_back(simInputScreen);

    PlaybackScreen* playbackScreen = new PlaybackScreen();
    this->guiTable[playbackScreen->name] = playbackScreen;
    this->guiList.push_back(playbackScreen);

    ProfilerScreen* profilerScreen = new ProfilerScreen();
    this->guiTable[profilerScreen->name] = profilerScreen;
    this->guiList.push_back(profilerScreen);

}

GUIManager::~GUIManager()
{
    for (int i = this->guiList.size() - 1; i >= 0; i--)
    {
        delete this->guiList[i];
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

GUIManager* GUIManager::getInstance()
{
	return sharedInstance;
}

void GUIManager::initialize(GLFWwindow* window)
{
	sharedInstance = new GUIManager(window);
}

void GUIManager::destroy()
{
	delete sharedInstance;
}
