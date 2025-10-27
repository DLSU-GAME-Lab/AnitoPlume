#include "TextureManager.hpp"
#include "vcl/vcl.hpp"

using namespace vcl;

TextureManager* TextureManager::sharedInstance = nullptr;

TextureManager::TextureManager()
{
	
}

TextureManager::~TextureManager()
{

}

TextureManager* TextureManager::getInstance()
{
	return sharedInstance;
}

void TextureManager::initialize()
{
	sharedInstance = new TextureManager();
}

void TextureManager::destroy()
{
	delete sharedInstance;
}

void TextureManager::loadWhiteTexture()
{
	const vcl::image_raw white{ 1,1,vcl::image_color_type::rgba,{255,255,255,255} };
	this->textureMap["white"] = vcl::create_texture_gpu(white);
}

void TextureManager::load(std::string name, std::string filePath)
{
	this->textureMap[name] = create_texture_gpu(image_load_png(filePath));
}

unsigned int TextureManager::getTexture(std::string name)
{
	return this->textureMap[name];
}
