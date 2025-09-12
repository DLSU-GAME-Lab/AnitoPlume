#pragma once
#include "vcl/opengl/opengl.hpp"
#include <string>
#include <unordered_map>

using namespace vcl;

class ShaderManager
{
private:
	std::unordered_map<std::string, GLuint> shaderMap;

public:
	static ShaderManager* getInstance();
	static void initialize();
	static void destroy();

	void load(std::string dirPath, std::string name, bool hasGeom = false);
	GLuint getShader(std::string name);

private:
	ShaderManager();
	~ShaderManager();
	ShaderManager(const ShaderManager&) {};
	ShaderManager operator=(const ShaderManager&) {};
	static ShaderManager* sharedInstance;
};