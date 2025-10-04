#pragma once
#include "vcl/shape/shape.hpp"
#include <string>
#include <unordered_map>

using namespace vcl;

class MeshManager
{
private:
	std::vector<mesh_drawable*> meshList;
	std::unordered_map<std::string, mesh_drawable*> meshMap;

public:
	static MeshManager* getInstance();
	static void initialize();
	static void destroy();

	void load(std::string name, std::string filePath);
	void loadPrimitive(std::string name, mesh& primitive);
	mesh_drawable* getMesh(std::string name);

private:
	MeshManager();
	~MeshManager();
	MeshManager(const MeshManager&) {};
	MeshManager operator=(const MeshManager&) {};
	static MeshManager* sharedInstance;
};