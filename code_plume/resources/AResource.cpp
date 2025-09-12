#include "AResource.hpp"

AResource::AResource(const wchar_t* fullPath)
{
	this->fullPath = fullPath;
}

AResource::~AResource()
{

}

std::wstring AResource::GetFilePath()
{
	return this->fullPath;
}
