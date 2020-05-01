#include "ProtoEnginePCH.h"
#include "ProtoUtils.h"

using namespace rapidxml;

void ProtoParser::XML::LoadComponents(xml_node<>* pComponents, GameObject* pCurr)
{
	Helper::LoadTransformComponent(pComponents, pCurr);
	Helper::LoadImageComponents(pComponents, pCurr);
	Helper::LoadTextComponents(pComponents, pCurr);
	Helper::LoadFPSComponents(pComponents, pCurr);

	if (ProtoSettings.GetRenderMode() == RenderMode::GAME)
		ProtoSettings.GetGame()->LoadComponents(pComponents, pCurr);

	else
		ProtoSettings.GetRefGame()->LoadComponents(pComponents, pCurr);
}

std::string ProtoParser::XML::ParseString(xml_node<>* pComp, const std::string& name)
{
	return std::string(pComp->first_attribute(name.c_str())->value());
}

void ProtoParser::XML::Helper::LoadTransformComponent(xml_node<>* pComponents, GameObject* pCurr)
{
	xml_node<>* pTransformNode{ pComponents->first_node("TransformComponent") };
	auto* pTransformComp{ pCurr->GetTransform() };

	glm::vec2 position;
	position.x = Parse<float>(pTransformNode, "PositionX");
	position.y = Parse<float>(pTransformNode, "PositionY");

	glm::vec2 rotCenter;
	rotCenter.x = Parse<float>(pTransformNode, "RotCenterX");
	rotCenter.y = Parse<float>(pTransformNode, "RotCenterY");

	const float rotAngle{ Parse<float>(pTransformNode, "RotAngle") };

	glm::vec2 scale;
	scale.x = Parse<float>(pTransformNode, "ScaleX");
	scale.y = Parse<float>(pTransformNode, "ScaleY");

	pTransformComp->SetPosition(position.x, position.y);
	pTransformComp->SetRotCenter(rotCenter.x, rotCenter.y);
	pTransformComp->SetRotAngle(rotAngle);
	pTransformComp->SetScale(scale.x, scale.y);
}

void ProtoParser::XML::Helper::LoadImageComponents(xml_node<>* pComponents, GameObject* pCurr)
{
	for (xml_node<>* pImageCompNode = pComponents->first_node("ImageComponent"); pImageCompNode; pImageCompNode = pImageCompNode->next_sibling())
	{
		const std::string imageLocation{ ParseString(pImageCompNode, "ImageLocation") };
		TextureData texData;
		LoadTexData(pImageCompNode, texData);

		HAlignment horAlignment;
		VAlignment verAlignment;
		LoadAlignments(pImageCompNode, horAlignment, verAlignment);

		auto pImageComp = new ImageComponent(ProtoContent.GetTexture(imageLocation), texData);
		pCurr->AddComponent(pImageComp);
		pImageComp->SetAlignment(horAlignment, verAlignment);
		pImageComp->SetTextureData(texData);
	}
}

void ProtoParser::XML::Helper::LoadTextComponents(xml_node<>* pComponents, GameObject* pCurr)
{
	for (xml_node<>* pTextCompNode = pComponents->first_node("TextComponent"); pTextCompNode; pTextCompNode = pTextCompNode->next_sibling())
	{
		const std::string fontLocation{ ParseString(pTextCompNode, "FontLocation") };
		const int fontSize{ Parse<int>(pTextCompNode, "FontSize") };

		TextureData texData;
		LoadTexData(pTextCompNode, texData);

		const std::string text{ ParseString(pTextCompNode, "Text") };

		HAlignment horAlignment;
		VAlignment verAlignment;
		LoadAlignments(pTextCompNode, horAlignment, verAlignment);

		auto pTextComp = new TextComponent(text, ProtoContent.GetFont(fontLocation, fontSize), texData);
		pCurr->AddComponent(pTextComp);
		pTextComp->SetAlignment(horAlignment, verAlignment);
		pTextComp->SetTextureData(texData);
	}
}

void ProtoParser::XML::Helper::LoadFPSComponents(xml_node<>* pComponents, GameObject* pCurr)
{
	for (xml_node<>* pFPSCompNode = pComponents->first_node("FPSComponent"); pFPSCompNode; pFPSCompNode = pFPSCompNode->next_sibling())
	{
		const std::string fontLocation{ ParseString(pFPSCompNode, "FontLocation") };
		const int fontSize{ Parse<int>(pFPSCompNode, "FontSize") };

		TextureData texData;
		LoadTexData(pFPSCompNode, texData);

		HAlignment horAlignment;
		VAlignment verAlignment;
		LoadAlignments(pFPSCompNode, horAlignment, verAlignment);

		auto pFPSComp = new FPSComponent(ProtoContent.GetFont(fontLocation, fontSize), texData);
		pCurr->AddComponent(pFPSComp);
		pFPSComp->SetAlignment(horAlignment, verAlignment);
		pFPSComp->SetTextureData(texData);
	}
}

void ProtoParser::XML::Helper::LoadTexData(xml_node<>* pComp, TextureData& texData)
{
	texData.x =			Parse<float>(pComp, "TexDataX");
	texData.y =			Parse<float>(pComp, "TexDataY");
	texData.width =		Parse<float>(pComp, "TexDataW");
	texData.height =	Parse<float>(pComp, "TexDataH");
	texData.color.r =	Parse<Uint8>(pComp, "TexDataColorR");
	texData.color.g =	Parse<Uint8>(pComp, "TexDataColorG");
	texData.color.b =	Parse<Uint8>(pComp, "TexDataColorB");
	texData.color.a =	Parse<Uint8>(pComp, "TexDataColorA");
}

void ProtoParser::XML::Helper::LoadAlignments(xml_node<>* pComp, HAlignment& horAlignment, VAlignment& verAlignment)
{
	const std::string horAlignmentStr{ ParseString(pComp, "HorizontalAlignment") };

	if (horAlignmentStr == "Left")
		horAlignment = HAlignment::LEFT;
	else if (horAlignmentStr == "Center")
		horAlignment = HAlignment::CENTER;
	else
		horAlignment = HAlignment::RIGHT;

	const std::string verAlignmentStr{ ParseString(pComp, "VerticalAlignment") };

	if (verAlignmentStr == "Top")
		verAlignment = VAlignment::TOP;
	else if (verAlignmentStr == "Center")
		verAlignment = VAlignment::CENTER;
	else
		verAlignment = VAlignment::BOTTOM;
}

float SecondsToMicroSeconds(float seconds)
{
	return std::roundf(seconds * 1'000'000);
}

float MicroSecondsToSeconds(float microSeconds)
{
	return microSeconds / 1'000'000;
}

std::string WStringToString(const std::wstring& wstring)
{
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;
	return converter.to_bytes(wstring);
}

std::wstring StringToWString(const std::string& string)
{
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;
	return converter.from_bytes(string);
}

void ProtoSaver::XML::SaveString(const char* name, std::string& value, xml_document<>& doc, xml_node<>* pComp)
{
	pComp->append_attribute(doc.allocate_attribute(name, doc.allocate_string(value.c_str())));
}
