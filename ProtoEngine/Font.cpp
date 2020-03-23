#include "ProtoEnginePCH.h"
#include <SDL_ttf.h>
#include "Font.h"

TTF_Font* Proto::Font::GetFont() const {
	return m_Font;
}

Proto::Font::Font(const std::string& fullPath, unsigned int size) : m_FullPath(fullPath), m_Font(nullptr)
{
	m_Font = TTF_OpenFont(fullPath.c_str(), int(size));
	if (m_Font == nullptr) 
	{
		throw std::runtime_error(std::string("Failed to load font: ") + SDL_GetError());
	}
}

Proto::Font::~Font()
{
	TTF_CloseFont(m_Font);
}

std::string Proto::Font::GetPath() const
{
	return m_FullPath;
}
