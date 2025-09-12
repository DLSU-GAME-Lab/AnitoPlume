#pragma once

class GUIManager
{
public:
	static GUIManager* getInstance();
	void initialize();
	void destroy();

private:
	GUIManager();
	~GUIManager();
	GUIManager(const GUIManager&) {};
	GUIManager operator=(const GUIManager&) {};
	static GUIManager* sharedInstance;
};