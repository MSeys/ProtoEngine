#include "ProtoEnginePCH.h"

#include "FPSComponent.h"

#include "Font.h"

FPSComponent::FPSComponent(Proto::Font* pFont, const TextureData& texData)
	: TextComponent("FPS", pFont, texData)
{
}

void FPSComponent::Update()
{
	m_Timer += ProtoTime.DeltaTime_Unscaled;
	if (m_Timer > 0.2f)
	{
		SetText(std::to_string(ProtoTime.FPS_Unscaled) + " FPS");
		m_Timer = 0.f;
	}
}

void FPSComponent::DrawInspectorTitle()
{
	ImGui::Text("FPS Component");
}

void FPSComponent::Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent)
{
	using namespace rapidxml;

	xml_node<>* pComp = doc.allocate_node(node_element, "FPSComponent");

	// Font related
	ProtoSaver::XML::SaveString("FontLocation", m_FontRelPath, doc, pComp);
	ProtoSaver::XML::Save<unsigned int>("FontSize", m_pFont->GetSize(), doc, pComp);

	// Texture Data related
	ProtoSaver::XML::Save<float>("TexDataX", m_TexData.x, doc, pComp);
	ProtoSaver::XML::Save<float>("TexDataY", m_TexData.y, doc, pComp);
	ProtoSaver::XML::Save<float>("TexDataW", m_TexData.width, doc, pComp);
	ProtoSaver::XML::Save<float>("TexDataH", m_TexData.height, doc, pComp);

	ProtoSaver::XML::Save<Uint8>("TexDataColorR", m_TexData.color.r, doc, pComp);
	ProtoSaver::XML::Save<Uint8>("TexDataColorG", m_TexData.color.g, doc, pComp);
	ProtoSaver::XML::Save<Uint8>("TexDataColorB", m_TexData.color.b, doc, pComp);
	ProtoSaver::XML::Save<Uint8>("TexDataColorA", m_TexData.color.a, doc, pComp);

	// Alignment related
	std::string hAlignmentStr, vAlignmentStr;
	switch (m_HorAlignment)
	{
	case HAlignment::LEFT:		hAlignmentStr = "Left"; break;
	case HAlignment::CENTER:	hAlignmentStr = "Center"; break;
	case HAlignment::RIGHT:		hAlignmentStr = "Right"; break;
	default:;
	}

	switch (m_VerAlignment)
	{
	case VAlignment::TOP:		vAlignmentStr = "Top"; break;
	case VAlignment::CENTER:	vAlignmentStr = "Center"; break;
	case VAlignment::BOTTOM:	vAlignmentStr = "Bottom"; break;
	default:;
	}

	ProtoSaver::XML::SaveString("HorizontalAlignment", hAlignmentStr, doc, pComp);
	ProtoSaver::XML::SaveString("VerticalAlignment", vAlignmentStr, doc, pComp);

	pParent->append_node(pComp);
}
