#include "ProtoEnginePCH.h"
#include "Image.h"

#include "Texture2D.h"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

Image::Image(ComponentID ID, bool isActive, Proto::Texture2D* pTexture, const glm::vec2& position, const glm::vec2& scale, const glm::vec2& pivot, const SDL_Color& color)
	: BaseBehaviour(ID, isActive), m_pTexture(pTexture)
	, m_Position(position), m_Scale(scale), m_Pivot(pivot), m_Color(color)
{
	if (!m_pTexture)
		return;

	m_TexRelPath = m_pTexture->GetRelativePath();

	int width, height;
	SDL_QueryTexture(m_pTexture->GetSDLTexture(), nullptr, nullptr, &width, &height);
	m_TextureSize.x = float(width);
	m_TextureSize.y = float(height);
}

void Image::SetTexture(const std::string& path)
{
	m_pTexture = ProtoContent.GetTexture(path);
	m_TexRelPath = path;

	int width, height;
	SDL_QueryTexture(m_pTexture->GetSDLTexture(), nullptr, nullptr, &width, &height);
	m_TextureSize.x = float(width);
	m_TextureSize.y = float(height);
}

void Image::Draw()
{
	if (!m_pTexture)
		return;

	RenderData data;
	data.originPosition = m_pGameObject->GetTransform()->GetPosition();
	data.originScale = m_pGameObject->GetTransform()->GetScale();
	data.originAngle = m_pGameObject->GetTransform()->GetRotation();
	
	data.position = m_Position;
	data.scale = m_Scale;
	data.pivot = m_Pivot;
	
	data.size = m_TextureSize;
	data.color = m_Color;

	ProtoRenderer.RenderTexture(*m_pTexture, data);
}

void Image::DrawInspectorTitle()
{
	DrawActiveCheckbox();
	ImGui::Text("Image");
}

void Image::DrawInspector()
{
	const std::string fullPath = fs::canonical(ProtoContent.GetDataPath()).string();
	const auto selection = ProtoGui::Presets::Path("Image", 100, m_TexRelPath, "Select an Image", fullPath, { "Image Files", "*.png *.jpg *.jpeg *.PNG" });;

	if (!selection.empty())
	{
		const std::string relPath{ selection[0].substr(fullPath.size()) };

		if (m_pTexture)
		{
			if (relPath != m_pTexture->GetRelativePath())
				SetTexture(relPath);
		}

		else
			SetTexture(relPath);
	}

	ImGui::SameLine();
	ProtoGui::Presets::Color(m_Color, m_Color);

	if (!m_pTexture)
		return;
	
	ProtoGui::Presets::InputXY({ "Position", "X", "Y" }, m_Position.x, m_Position.y, { 1.f, 0, 0, "%.0f" }, 0);
	ProtoGui::Presets::InputXY({ "Scale", "X", "Y" }, m_Scale.x, m_Scale.y, { 0.01f, 0, 0, "%.2f" }, 1);
	ProtoGui::Presets::InputXY({ "Pivot", "X", "Y" }, m_Pivot.x, m_Pivot.y, { 0.1f, 0, 1, "%.1f" }, 2);
}

void Image::Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent)
{
	using namespace rapidxml;

	xml_node<>* pComp = doc.allocate_node(node_element, doc.allocate_string(ProtoConvert::ToString<Image>().c_str()));
	SaveID(doc, pComp);
	SaveActive(doc, pComp);

	ProtoSaver::XML::SaveString("ImageLocation", m_TexRelPath, doc, pComp);

	ProtoSaver::XML::Save<float>("PositionX", m_Position.x, doc, pComp);
	ProtoSaver::XML::Save<float>("PositionY", m_Position.y, doc, pComp);
	
	ProtoSaver::XML::Save<float>("ScaleX", m_Scale.x, doc, pComp);
	ProtoSaver::XML::Save<float>("ScaleY", m_Scale.y, doc, pComp);
	
	ProtoSaver::XML::Save<float>("PivotX", m_Pivot.x, doc, pComp);
	ProtoSaver::XML::Save<float>("PivotY", m_Pivot.y, doc, pComp);
	
	ProtoSaver::XML::Save<Uint8>("ColorR", m_Color.r, doc, pComp);
	ProtoSaver::XML::Save<Uint8>("ColorG", m_Color.g, doc, pComp);
	ProtoSaver::XML::Save<Uint8>("ColorB", m_Color.b, doc, pComp);
	ProtoSaver::XML::Save<Uint8>("ColorA", m_Color.a, doc, pComp);

	pParent->append_node(pComp);
}

void Image::Load(rapidxml::xml_node<>* pComp, GameObject* pCurr)
{
	const auto id{ ProtoParser::XML::Parse<unsigned int>(pComp, "ID") };
	const auto isActive{ ProtoParser::XML::Parse<bool>(pComp, "Active") };

	const std::string imageLocation{ ProtoParser::XML::ParseString(pComp, "ImageLocation") };

	const glm::vec2 position
	{
		ProtoParser::XML::Parse<float>(pComp, "PositionX"),
		ProtoParser::XML::Parse<float>(pComp, "PositionY")
	};

	const glm::vec2 scale
	{
		ProtoParser::XML::Parse<float>(pComp, "ScaleX"),
		ProtoParser::XML::Parse<float>(pComp, "ScaleY")
	};

	const glm::vec2 pivot
	{
		ProtoParser::XML::Parse<float>(pComp, "PivotX"),
		ProtoParser::XML::Parse<float>(pComp, "PivotY")
	};

	const SDL_Color color
	{
		ProtoParser::XML::Parse<Uint8>(pComp, "ColorR"),
		ProtoParser::XML::Parse<Uint8>(pComp, "ColorG"),
		ProtoParser::XML::Parse<Uint8>(pComp, "ColorB"),
		ProtoParser::XML::Parse<Uint8>(pComp, "ColorA")
	};

	Proto::Texture2D* pTexture{ imageLocation.empty() ? nullptr : ProtoContent.GetTexture(imageLocation) };
	const auto pSpriteComp = new Image(id, isActive, pTexture, position, scale, pivot, color);
	pCurr->AddComponent(pSpriteComp);
}