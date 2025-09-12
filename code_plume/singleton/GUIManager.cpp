#include "GUIManager.hpp"
#include "vcl/opengl/opengl.hpp"

GUIManager* GUIManager::sharedInstance = nullptr;

GUIManager::GUIManager()
{

}

GUIManager::~GUIManager()
{

}

GUIManager* GUIManager::getInstance()
{
	return sharedInstance;
}

void GUIManager::initialize()
{
	sharedInstance = new GUIManager();
}

void GUIManager::destroy()
{
	delete sharedInstance;
}