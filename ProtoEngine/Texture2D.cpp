#include "ProtoEnginePCH.h"
#include "Texture2D.h"
#include <SDL.h>

#include <utility>

Proto::Texture2D::~Texture2D()
{
	SDL_DestroyTexture(m_Texture);
}

std::string Proto::Texture2D::GetFullPath() const
{
	return m_FullPath;
}

std::string Proto::Texture2D::GetRelativePath() const
{
	return m_RelativePath;
}

SDL_Texture* Proto::Texture2D::GetSDLTexture() const
{
	return m_Texture;
}

Proto::Texture2D::Texture2D(SDL_Texture* texture, std::string fullPath, std::string relativePath)
	: m_FullPath(std::move(fullPath)), m_RelativePath(std::move(relativePath)), m_Texture(texture)
{
}
