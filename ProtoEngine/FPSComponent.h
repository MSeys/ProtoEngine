#pragma once
#include "TextComponent.h"

class FPSComponent : public TextComponent
{
public:
	FPSComponent(Proto::Font* pFont, const Proto::TextureData& texData);
	
protected:
	void Update() override;
	void FixedUpdate() override;
	
	void DrawInspectorTitle() override;

private:
	float m_Timer{ 0.21f };
};