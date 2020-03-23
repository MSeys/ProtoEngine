#include "ProtoEnginePCH.h"
#include "TextComponent.h"


#include <SDL_ttf.h>
#include <utility>
#include "Font.h"
#include "Texture2D.h"
#include "Utils.h"

TextComponent::TextComponent(std::string text, Proto::Font* pFont, const Proto::TextureData& texData)
	: RenderComponent(nullptr, texData), m_Text(std::move(text)), m_pFont(pFont), m_NeedsUpdate(false)
{
}

TextComponent::~TextComponent()
{
	SafeDelete(m_pFont);
}

void TextComponent::SetText(const std::string& text)
{
	m_Text = text;
	m_NeedsUpdate = true;
}

void TextComponent::SetFontSize(unsigned size)
{
	const std::string fullPath{ m_pFont->GetPath() };
	SafeDelete(m_pFont);
	m_pFont = ProtoResources.LoadFont(fullPath, size);
	m_NeedsUpdate = true;
}

void TextComponent::Initialize()
{
	SafeDelete(m_pTexture);
	
	const auto surf = TTF_RenderText_Blended(m_pFont->GetFont(), m_Text.c_str(), m_TexData.color);
	if (!surf)
		throw std::runtime_error(std::string("Render text failed: ") + SDL_GetError());

	const auto texture = SDL_CreateTextureFromSurface(ProtoRenderer.GetSDLRenderer(), surf);
	if (!texture)
		throw std::runtime_error(std::string("Create text texture from surface failed: ") + SDL_GetError());
	
	SDL_FreeSurface(surf);
	
	m_pTexture = new Proto::Texture2D(texture);
}

void TextComponent::Update()
{
	if (m_NeedsUpdate)
	{
		Initialize();
		m_NeedsUpdate = false;
	}
}