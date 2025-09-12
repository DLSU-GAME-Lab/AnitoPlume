#include "AResourceManager.hpp"
#include <filesystem>

AResource* AResourceManager::CreateResourceFromFile(const wchar_t* filePath)
{
	std::wstring fullPath = std::filesystem::absolute(filePath);

	auto it = this->resourceMap.find(fullPath);

	if (it != this->resourceMap.end())
		return it->second;

	AResource* rawRes = this->CreateResourceFromFileConcrete(fullPath.c_str());

	if (rawRes)
	{
		this->resourceMap[fullPath] = rawRes;
		return rawRes;
	}

	return nullptr;
}

AResourceManager::AResourceManager()
{

}

AResourceManager::~AResourceManager()
{

}

void AResourceManager::FreeResources()
{
	for (auto it = this->resourceMap.begin(); it != this->resourceMap.end(); it++)
		delete it->second;

	this->resourceMap.clear();
}
