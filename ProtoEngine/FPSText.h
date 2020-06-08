#pragma once
#include "Text.h"

class FPSText final : public Text
{
public:
	FPSText(ComponentID ID, bool isActive, Proto::Font* pFont, const TextureData& texData = { 0, 0 });
	
protected:
	void Update() override;
	
	void DrawInspectorTitle() override;

	void Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent) override;
	
private:
	float m_Timer{ 0.21f };

public:
	static void Load(rapidxml::xml_node<>* pComp, GameObject* pCurr);
};