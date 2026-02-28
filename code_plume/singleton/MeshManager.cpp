#include "MeshManager.hpp"

MeshManager* MeshManager::sharedInstance = nullptr;

MeshManager::MeshManager()
{

}

MeshManager::~MeshManager()
{

}

MeshManager* MeshManager::getInstance()
{
	return sharedInstance;
}

void MeshManager::initialize()
{
	sharedInstance = new MeshManager();
}

void MeshManager::destroy()
{
	delete sharedInstance;
}

void MeshManager::load(std::string name, std::string filePath)
{
	mesh mesh = vcl::mesh_load_file_obj(filePath);
	this->meshMap[name] = new mesh_drawable(mesh);
}

void MeshManager::loadPrimitive(std::string name, mesh& primitive)
{
	this->meshMap[name] = new mesh_drawable(primitive);
}

mesh_drawable* MeshManager::getMesh(std::string name)
{
	return this->meshMap[name];
}
