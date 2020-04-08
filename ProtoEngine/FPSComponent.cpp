#include "ProtoEnginePCH.h"

#include "FPSComponent.h"

#include "Font.h"

FPSComponent::FPSComponent(Proto::Font* pFont, const Proto::TextureData& texData)
	: TextComponent("FPS", pFont, texData)
{
}

void FPSComponent::Update()
{
	m_Timer += ProtoTime.DeltaTime;
	if (m_Timer > 0.2f)
	{
		SetText(std::to_string(ProtoTime.FPS) + " FPS");
		m_Timer = 0.f;
	}
}

void FPSComponent::FixedUpdate()
{
}

void FPSComponent::DrawInspectorTitle()
{
	ImGui::Text("FPS Component");
}