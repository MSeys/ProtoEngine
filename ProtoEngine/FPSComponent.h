#pragma once
#include "TextComponent.h"

class FPSComponent : public TextComponent
{
public:
	FPSComponent(Proto::Font* pFont, const TextureData& texData);
	
protected:
	void Update() override;
	
	void DrawInspectorTitle() override;

	void Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent) override;
private:
	float m_Timer{ 0.21f };
};