#pragma once
#include <string>
#include <unordered_map>

class TextureManager
{
private:
	std::unordered_map<std::string, unsigned int> textureMap;

public:
	static TextureManager* getInstance();
	static void initialize();
	static void destroy();

	void loadWhiteTexture();
	void load(std::string name, std::string filePath);
	unsigned int getTexture(std::string name);

private:
	TextureManager();
	~TextureManager();
	TextureManager(const TextureManager&) {};
	TextureManager operator=(const TextureManager&) {};
	static TextureManager* sharedInstance;
};