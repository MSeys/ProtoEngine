#include "ProtoEnginePCH.h"

#include "FPSComponent.h"

#include "Font.h"

FPSComponent::FPSComponent(Proto::Font* pFont, const TextureData& texData)
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

void FPSComponent::DrawInspectorTitle()
{
	ImGui::Text("FPS Component");
}

void FPSComponent::Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent)
{
	using namespace rapidxml;

	xml_node<>* component = doc.allocate_node(node_element, "FPSComponent");

	// Font related
	component->append_attribute(doc.allocate_attribute("FontLocation", doc.allocate_string(m_FontRelPath.c_str())));
	component->append_attribute(doc.allocate_attribute("FontSize", doc.allocate_string(ToCString(m_pFont->GetSize()))));

	// Texture Data related
	component->append_attribute(doc.allocate_attribute("TexDataX", doc.allocate_string(ToCString(m_TexData.x))));
	component->append_attribute(doc.allocate_attribute("TexDataY", doc.allocate_string(ToCString(m_TexData.y))));
	component->append_attribute(doc.allocate_attribute("TexDataW", doc.allocate_string(ToCString(m_TexData.width))));
	component->append_attribute(doc.allocate_attribute("TexDataH", doc.allocate_string(ToCString(m_TexData.height))));
	component->append_attribute(doc.allocate_attribute("TexDataColorR", doc.allocate_string(ToCString(m_TexData.color.r))));
	component->append_attribute(doc.allocate_attribute("TexDataColorG", doc.allocate_string(ToCString(m_TexData.color.g))));
	component->append_attribute(doc.allocate_attribute("TexDataColorB", doc.allocate_string(ToCString(m_TexData.color.b))));
	component->append_attribute(doc.allocate_attribute("TexDataColorA", doc.allocate_string(ToCString(m_TexData.color.a))));

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

	component->append_attribute(doc.allocate_attribute("HorizontalAlignment", doc.allocate_string(hAlignmentStr.c_str())));
	component->append_attribute(doc.allocate_attribute("VerticalAlignment", doc.allocate_string(vAlignmentStr.c_str())));

	pParent->append_node(component);
}
