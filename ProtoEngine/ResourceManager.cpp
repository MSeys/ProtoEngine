#include "ProtoEnginePCH.h"
#include "ResourceManager.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "Renderer.h"
#include "Texture2D.h"
#include "Font.h"

#include <fstream>

void Proto::ResourceManager::Init(const std::string& dataPath)
{
	m_DataPath = dataPath;

	// load support for png and jpg, this takes a while!

	if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) 
	{
		throw std::runtime_error(std::string("Failed to load support for png's: ") + SDL_GetError());
	}

	if ((IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG) != IMG_INIT_JPG) 
	{
		throw std::runtime_error(std::string("Failed to load support for jpg's: ") + SDL_GetError());
	}

	if (TTF_Init() != 0) 
	{
		throw std::runtime_error(std::string("Failed to load support for fonts: ") + SDL_GetError());
	}
}

Proto::Texture2D* Proto::ResourceManager::LoadTexture(const std::string& file) const
{
	const auto fullPath{ m_DataPath + file };
	const auto texture{ IMG_LoadTexture(Renderer::GetInstance().GetSDLRenderer(), fullPath.c_str()) };
	if (!texture) 
	{
		throw std::runtime_error(std::string("Failed to load texture: ") + SDL_GetError());
	}
	
	return new Texture2D(texture, fullPath, file);
}

Proto::Font* Proto::ResourceManager::LoadFont(const std::string& file, unsigned int size) const
{
	return new Font(m_DataPath + file, file, size);
}

bool Proto::ResourceManager::FileExists(const std::string& file, const ResourceType& type) const
{
	if(type == ResourceType::FONT)
	{
		const std::array<std::string, 2> formats{ "ttf", "otf" };
		auto it = std::find_if(formats.cbegin(), formats.cend(), [&](const std::string& s) {return file.find(s) != std::string::npos; });

		if (it == formats.end())
			return false;

		const std::ifstream f(m_DataPath + file);
		return f.good();
	}

	if (type == ResourceType::IMAGE)
	{
		const std::array<std::string, 4> formats{ "jpeg", "png", "jpg", "PNG" };
		auto it = std::find_if(formats.cbegin(), formats.cend(), [&](const std::string& s) {return file.find(s) != std::string::npos; });

		if (it == formats.end())
			return false;

		const std::ifstream f(m_DataPath + file);
		return f.good();
	}

	const std::ifstream f(m_DataPath + file);
	return f.good();
}
