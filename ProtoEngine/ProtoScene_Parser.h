// ReSharper disable CppClangTidyCertErr34C

#pragma once
#include "GameObject.h"
#include "Components.h"

#pragma warning(push)
#pragma warning (disable:4201)
#include <glm/glm.hpp>
#pragma warning(pop)

#include <string>

void LoadComponents(rapidxml::xml_node<>* pComponents, GameObject* pCurr);
void LoadTransformComponent(rapidxml::xml_node<>* pComponents, GameObject* pCurr);
void LoadImageComponents(rapidxml::xml_node<>* pComponents, GameObject* pCurr);
void LoadTextComponents(rapidxml::xml_node<>* pComponents, GameObject* pCurr);
void LoadFPSComponents(rapidxml::xml_node<>* pComponents, GameObject* pCurr);


void LoadTexData(rapidxml::xml_node<>* pComp, TextureData& texData);
void LoadAlignments(rapidxml::xml_node<>* pComp, HAlignment& horAlignment, VAlignment& verAlignment);


inline void LoadComponents(rapidxml::xml_node<>* pComponents, GameObject* pCurr)
{
	LoadTransformComponent(pComponents, pCurr);
	LoadImageComponents(pComponents, pCurr);
	LoadTextComponents(pComponents, pCurr);
	LoadFPSComponents(pComponents, pCurr);
}

inline void LoadTransformComponent(rapidxml::xml_node<>* pComponents, GameObject* pCurr)
{
	using namespace rapidxml;
	
	xml_node<>* pTransformNode = pComponents->first_node("TransformComponent");
	auto pTransformComp = pCurr->GetTransform();

	glm::vec2 position;
	position.x = stof(std::string(pTransformNode->first_attribute("PositionX")->value()));
	position.y = stof(std::string(pTransformNode->first_attribute("PositionY")->value()));

	glm::vec2 rotCenter;
	rotCenter.x = stof(std::string(pTransformNode->first_attribute("RotCenterX")->value()));
	rotCenter.y = stof(std::string(pTransformNode->first_attribute("RotCenterY")->value()));
	
	const float rotAngle = stof(std::string(pTransformNode->first_attribute("RotAngle")->value()));

	glm::vec2 scale;
	scale.x = stof(std::string(pTransformNode->first_attribute("ScaleX")->value()));
	scale.y = stof(std::string(pTransformNode->first_attribute("ScaleY")->value()));
	
	pTransformComp->SetPosition(position.x, position.y);
	pTransformComp->SetRotCenter(rotCenter.x, rotCenter.y);
	pTransformComp->SetRotAngle(rotAngle);
	pTransformComp->SetScale(scale.x, scale.y);
}

inline void LoadImageComponents(rapidxml::xml_node<>* pComponents, GameObject* pCurr)
{
	using namespace rapidxml;

	for (xml_node<>* pImageCompNode = pComponents->first_node("ImageComponent"); pImageCompNode; pImageCompNode = pImageCompNode->next_sibling())
	{
		const std::string imageLocation{ pImageCompNode->first_attribute("ImageLocation")->value() };
		TextureData texData;
		LoadTexData(pImageCompNode, texData);

		HAlignment horAlignment;
		VAlignment verAlignment;
		LoadAlignments(pImageCompNode, horAlignment, verAlignment);

		auto pImageComp = new ImageComponent(ProtoContent.GetTexture(imageLocation), texData);
		pCurr->AddComponent(pImageComp);
		pImageComp->SetAlignment(horAlignment, verAlignment);
	}
}

inline void LoadTextComponents(rapidxml::xml_node<>* pComponents, GameObject* pCurr)
{
	using namespace rapidxml;

	for (xml_node<>* pTextCompNode = pComponents->first_node("TextComponent"); pTextCompNode; pTextCompNode = pTextCompNode->next_sibling())
	{
		const std::string fontLocation{ pTextCompNode->first_attribute("FontLocation")->value() };
		const int fontSize{ stoi(std::string(pTextCompNode->first_attribute("FontSize")->value())) };
		
		TextureData texData;
		LoadTexData(pTextCompNode, texData);

		const std::string text{ pTextCompNode->first_attribute("Text")->value() };
		
		HAlignment horAlignment;
		VAlignment verAlignment;
		LoadAlignments(pTextCompNode, horAlignment, verAlignment);

		auto pTextComp = new TextComponent(text, ProtoContent.GetFont(fontLocation, fontSize), texData);
		pCurr->AddComponent(pTextComp);
		pTextComp->SetAlignment(horAlignment, verAlignment);
	}
}

inline void LoadFPSComponents(rapidxml::xml_node<>* pComponents, GameObject* pCurr)
{
	using namespace rapidxml;

	for (xml_node<>* pFPSCompNode = pComponents->first_node("FPSComponent"); pFPSCompNode; pFPSCompNode = pFPSCompNode->next_sibling())
	{
		const std::string fontLocation{ pFPSCompNode->first_attribute("FontLocation")->value() };
		const int fontSize{ stoi(std::string(pFPSCompNode->first_attribute("FontSize")->value())) };

		TextureData texData;
		LoadTexData(pFPSCompNode, texData);

		HAlignment horAlignment;
		VAlignment verAlignment;
		LoadAlignments(pFPSCompNode, horAlignment, verAlignment);

		auto pFPSComp = new FPSComponent(ProtoContent.GetFont(fontLocation, fontSize), texData);
		pCurr->AddComponent(pFPSComp);
		pFPSComp->SetAlignment(horAlignment, verAlignment);
	}
}


#pragma region Helper Load Functions
inline void LoadTexData(rapidxml::xml_node<>* pComp, TextureData& texData)
{
	texData.x = stof(std::string(pComp->first_attribute("TexDataX")->value()));
	texData.y = stof(std::string(pComp->first_attribute("TexDataY")->value()));
	texData.width = stof(std::string(pComp->first_attribute("TexDataW")->value()));
	texData.height = stof(std::string(pComp->first_attribute("TexDataH")->value()));
	texData.color.r = Uint8(stoi(std::string(pComp->first_attribute("TexDataColorR")->value())));
	texData.color.g = Uint8(stoi(std::string(pComp->first_attribute("TexDataColorG")->value())));
	texData.color.b = Uint8(stoi(std::string(pComp->first_attribute("TexDataColorB")->value())));
	texData.color.a = Uint8(stoi(std::string(pComp->first_attribute("TexDataColorA")->value())));
}

inline void LoadAlignments(rapidxml::xml_node<>* pComp, HAlignment& horAlignment, VAlignment& verAlignment)
{
	const std::string horAlignmentStr{ pComp->first_attribute("HorizontalAlignment")->value() };

	if (horAlignmentStr == "Left")
		horAlignment = HAlignment::LEFT;
	else if (horAlignmentStr == "Center")
		horAlignment = HAlignment::CENTER;
	else
		horAlignment = HAlignment::RIGHT;

	const std::string verAlignmentStr{ pComp->first_attribute("VerticalAlignment")->value() };

	if (verAlignmentStr == "Top")
		verAlignment = VAlignment::TOP;
	else if (verAlignmentStr == "Center")
		verAlignment = VAlignment::CENTER;
	else
		verAlignment = VAlignment::BOTTOM;
}

#pragma endregion Helper Load Functions