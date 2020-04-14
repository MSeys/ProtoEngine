#include "ProtoEnginePCH.h"
#include "ContentManager.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "Renderer.h"
#include "Texture2D.h"
#include "Font.h"

void Proto::ContentManager::Init(const std::string& data)
{
	m_DataPath = data;

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

void Proto::ContentManager::Destroy()
{
	for (auto& pair : m_pTextures)
		SafeDelete(pair.second);

	for (auto& pair : m_pFonts)
		SafeDelete(pair.second);
}

Proto::Texture2D* Proto::ContentManager::GetTexture(const std::string& file)
{
	if(m_pTextures.find(file) == m_pTextures.end())
		LoadTexture(file);

	return m_pTextures[file];
}

Proto::Font* Proto::ContentManager::GetFont(const std::string& file, unsigned int size)
{
	const std::string key{ file + "_" + std::to_string(size) };

	if (m_pFonts.find(key) == m_pFonts.end())
		LoadFont(file, size);

	return m_pFonts[key];
}

void Proto::ContentManager::LoadTexture(const std::string& file)
{
	const auto fullPath{ m_DataPath + file };
	const auto texture{ IMG_LoadTexture(Renderer::GetInstance().GetSDLRenderer(), fullPath.c_str()) };
	if (!texture)
	{
		throw std::runtime_error(std::string("Failed to load texture: ") + SDL_GetError());
	}

	m_pTextures[file] = new Texture2D(texture, fullPath, file);
}

void Proto::ContentManager::LoadFont(const std::string& file, unsigned int size)
{
	const std::string key{ file + "_" + std::to_string(size) };
	m_pFonts[key] = new Font(m_DataPath + file, file, size);
}

std::string Proto::ContentManager::GetDataPath() const
{
	return m_DataPath;
}
