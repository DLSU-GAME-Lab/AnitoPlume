#pragma once
#include <string>
#include <unordered_map>

class ShaderManager
{
private:
	std::unordered_map<std::string, unsigned int> shaderMap;

public:
	static ShaderManager* getInstance();
	static void initialize();
	static void destroy();

	void load(std::string dirPath, std::string name, bool hasGeom = false);
	unsigned int getShader(std::string name);

private:
	ShaderManager();
	~ShaderManager();
	ShaderManager(const ShaderManager&) {};
	ShaderManager operator=(const ShaderManager&) {};
	static ShaderManager* sharedInstance;
};