#pragma once

class GUIManager
{
public:
	static GUIManager* getInstance();
	static void initialize();
	static void destroy();

private:
	GUIManager();
	~GUIManager();
	GUIManager(const GUIManager&) {};
	GUIManager operator=(const GUIManager&) {};
	static GUIManager* sharedInstance;
};