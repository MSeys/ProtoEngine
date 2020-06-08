#include "ProtoEnginePCH.h"

#include "FPSText.h"

#include "Font.h"

FPSText::FPSText(ComponentID ID, bool isActive, Proto::Font* pFont, const TextureData& texData)
	: Text(ID, isActive, "FPS", pFont, texData)
{
}

void FPSText::Update()
{
	m_Timer += ProtoTime.DeltaTime_Unscaled;
	if (m_Timer > 0.2f)
	{
		SetText(std::to_string(ProtoTime.FPS_Unscaled) + " FPS");
		m_Timer = 0.f;
	}
}

void FPSText::DrawInspectorTitle()
{
	DrawActiveCheckbox();
	ImGui::Text("FPS Text");
}

void FPSText::Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent)
{
	using namespace rapidxml;

	xml_node<>* pComp = doc.allocate_node(node_element, doc.allocate_string(ProtoConvert::ToString<FPSText>().c_str()));

	SaveID(doc, pComp);
	SaveActive(doc, pComp);
	
	// Font related
	ProtoSaver::XML::SaveString("FontLocation", m_FontRelPath, doc, pComp);

	auto fontSize{ m_pFont ? m_pFont->GetSize() : 13 };
	ProtoSaver::XML::Save<unsigned int>("FontSize", fontSize, doc, pComp);

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

void FPSText::Load(rapidxml::xml_node<>* pComp, GameObject* pCurr)
{
	const auto id{ ProtoParser::XML::Parse<unsigned int>(pComp, "ID") };
	const auto isActive{ ProtoParser::XML::Parse<bool>(pComp, "Active") };

	const std::string fontLocation{ ProtoParser::XML::ParseString(pComp, "FontLocation") };
	const int fontSize{ ProtoParser::XML::Parse<int>(pComp, "FontSize") };

	TextureData texData;
	ProtoParser::XML::Helper::LoadTexData(pComp, texData);

	HAlignment horAlignment;
	VAlignment verAlignment;
	ProtoParser::XML::Helper::LoadAlignments(pComp, horAlignment, verAlignment);

	const auto pFont = !fontLocation.empty() ? ProtoContent.GetFont(fontLocation, fontSize) : nullptr;
	auto pFPSComp = new FPSText(id, isActive, pFont, texData);
	pCurr->AddComponent(pFPSComp);
	pFPSComp->SetAlignment(horAlignment, verAlignment);
	pFPSComp->SetTextureData(texData);
}
