#pragma once

#include "AResource.hpp"
#include <unordered_map>
#include <string>

class AResourceManager
{
protected:
	AResourceManager();
	~AResourceManager();
	void FreeResources();

	AResource* CreateResourceFromFile(const wchar_t* filePath);
	virtual AResource* CreateResourceFromFileConcrete(const wchar_t* filePath) = 0;

private:
	std::unordered_map<std::wstring, AResource*> resourceMap;
};

