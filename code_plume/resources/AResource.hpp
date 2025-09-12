#pragma once
#include <string>

class AResource
{
public:
	AResource(const wchar_t* fullPath);
	virtual ~AResource() = 0;

	std::wstring GetFilePath();

protected:
	std::wstring fullPath;
};

