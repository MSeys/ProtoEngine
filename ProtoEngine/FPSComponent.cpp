#include "ProtoEnginePCH.h"

#include "FPSComponent.h"

FPSComponent::FPSComponent(Proto::Font* pFont, const Proto::TextureData& texData)
	: TextComponent("FPS", pFont, texData)
{
}

void FPSComponent::Update()
{
	m_Timer += ProtoTime.DeltaTime;
	if (m_Timer > 0.2f)
	{
		TextComponent::Update();
		SetText(std::to_string(ProtoTime.FPS) + " FPS");
		m_Timer = 0.f;
	}
}