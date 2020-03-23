#include "ProtoEnginePCH.h"
#include "RenderComponent.h"

#include "Texture2D.h"
#include "Utils.h"

RenderComponent::RenderComponent(Proto::Texture2D* pTexture, const Proto::TextureData& texData)
	: m_pTexture(pTexture), m_TexData(texData)
{
}

RenderComponent::~RenderComponent()
{
	SafeDelete(m_pTexture);
}

void RenderComponent::SetTextureData(const Proto::TextureData& texData)
{
	m_TexData = texData;
}

void RenderComponent::Initialize()
{
}

void RenderComponent::Update()
{
}

void RenderComponent::Draw()
{
	if(m_TexData.width == -1 || m_TexData.height == -1)
		ProtoRenderer.RenderTexture(*m_pTexture, m_TexData.x, m_TexData.y);

	else
		ProtoRenderer.RenderTexture(*m_pTexture, m_TexData.x, m_TexData.y, m_TexData.width, m_TexData.height);
}
