#include "ProtoEnginePCH.h"
#include "Texture2D.h"
#include <SDL.h>

Proto::Texture2D::~Texture2D()
{
	SDL_DestroyTexture(m_Texture);
}

SDL_Texture* Proto::Texture2D::GetSDLTexture() const
{
	return m_Texture;
}

Proto::Texture2D::Texture2D(SDL_Texture* texture)
{
	m_Texture = texture;
}
