#include "ShaderManager.hpp"

ShaderManager* ShaderManager::sharedInstance = nullptr;

ShaderManager::ShaderManager()
{

}

ShaderManager::~ShaderManager()
{

}

ShaderManager* ShaderManager::getInstance()
{
	return sharedInstance;
}

void ShaderManager::initialize()
{
	sharedInstance = new ShaderManager();
}

void ShaderManager::destroy()
{
	delete sharedInstance;
}

void ShaderManager::load(std::string dirPath, std::string name, bool hasGeom)
{
	if (hasGeom)
	{
		this->shaderMap[name] = vcl::create_shader_program(
			"shaders/" + dirPath + "/shader.vert.glsl",
			"shaders/" + dirPath + "/shader.geom.glsl",
			"shaders/" + dirPath + "/shader.frag.glsl");
	}
	else
	{
		this->shaderMap[name] = vcl::create_shader_program(
			"shaders/" + dirPath + "/shader.vert.glsl",
			"shaders/" + dirPath + "/shader.frag.glsl");
	}
}

GLuint ShaderManager::getShader(std::string name)
{
	return this->shaderMap[name];
}
