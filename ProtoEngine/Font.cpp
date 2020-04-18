#include "ProtoEnginePCH.h"
#include <SDL_ttf.h>

#include <utility>
#include "Font.h"

TTF_Font* Proto::Font::GetFont() const {
	return m_Font;
}

Proto::Font::Font(const std::string& fullPath, std::string relativePath, unsigned int size) : m_FullPath(fullPath), m_RelativePath(std::move(relativePath)), m_Font(nullptr), m_Size(size)
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

std::string Proto::Font::GetFullPath() const
{
	return m_FullPath;
}

std::string Proto::Font::GetRelativePath() const
{
	return m_RelativePath;
}

unsigned& Proto::Font::GetSize()
{
	return m_Size;
}
