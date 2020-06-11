#include "ProtoEnginePCH.h"
#include "Text.h"

#include <SDL_ttf.h>
#include <utility>
#include "Font.h"
#include "Texture2D.h"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

Text::Text(ComponentID ID, bool isActive, std::string text, Proto::Font* pFont, const glm::vec2& position, const glm::vec2& scale, const glm::vec2& pivot, SDL_Color color)
	: BaseBehaviour(ID, isActive), m_pFont(pFont), m_Text(std::move(text))
	, m_Position(position), m_Scale(scale), m_Pivot(pivot), m_Color(color)
{
	if (!pFont)
		return;
	
	m_FontRelPath = pFont->GetRelativePath();
	UpdateText();
}

Text::~Text()
{
	SafeDelete(m_pTexture);
}

void Text::SetText(const std::string& text)
{
	m_Text = text;
	UpdateText();
}

void Text::SetFontSize(unsigned size)
{
	const std::string relPath{ m_pFont->GetRelativePath() };
	m_pFont = ProtoContent.GetFont(relPath, size);
	
	UpdateText();
}

void Text::SetFont(const std::string& path)
{
	unsigned int size{ 25 };
	if(m_pFont)
		size = m_pFont->GetSize();
	
	m_pFont = ProtoContent.GetFont(path, size);
	m_FontRelPath = path;

	UpdateText();
}

void Text::DrawInspectorTitle()
{
	DrawActiveCheckbox();
	ImGui::Text("Text");
}

void Text::DrawInspector()
{
	const std::string fullPath = fs::canonical(ProtoContent.GetDataPath()).string();
	const auto selection = ProtoGui::Presets::Path("Font", 100, m_FontRelPath, "Select a Font", fullPath, { "Font Files", "*.ttf *.otf" });

	if (!selection.empty())
	{
		const std::string relPath{ selection[0].substr(fullPath.size()) };
		
		if (m_pFont)
		{
			if (relPath != m_pFont->GetRelativePath())
				SetFont(relPath);
		}

		else
			SetFont(relPath);
	}

	if (!m_pFont)
		return;

	ProtoGui::Presets::InputXY({ "Position", "X", "Y" }, m_Position.x, m_Position.y, { 1.f, 0, 0, "%.0f" }, 0);
	ProtoGui::Presets::InputXY({ "Scale", "X", "Y" }, m_Scale.x, m_Scale.y, { 0.01f, 0, 0, "%.2f" }, 1);
	ProtoGui::Presets::InputXY({ "Pivot", "X", "Y" }, m_Pivot.x, m_Pivot.y, { 0.1f, 0, 1, "%.1f" }, 2);
	
	/* Font Size */ {
		ImGui::Text("Font Size");
		ImGui::SameLine(100);

		int newSize{ int(m_pFont->GetSize()) };

		ImGui::PushItemWidth(250);
		ImGui::DragInt("##TEXT_COMP_FONT_SIZE", &newSize, 1, 10, 100);
		ImGui::PopItemWidth();

		if (newSize != int(m_pFont->GetSize()))
			SetFontSize(newSize);
	}

	/* Text */ {
		m_Text.resize(200);

		ImGui::Text("Text");
		ImGui::SameLine(100);

		std::string newText{ m_Text };
		newText.resize(m_Text.size());
		
		ImGui::PushItemWidth(225);
		ImGui::InputText("##TEXT_COMP_TEXT", &newText[0], 200);
		ImGui::PopItemWidth();

		if (newText != m_Text)
		{
			SetText(newText);
		}
	}

	ImGui::SameLine();
	SDL_Color newColor{};
	ProtoGui::Presets::Color(m_Color, newColor);

	if (newColor.r != m_Color.r || newColor.g != m_Color.g || newColor.b != m_Color.b || newColor.a != m_Color.a)
		SetColor(newColor);
}

void Text::Draw()
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
	data.color = { 255, 255, 255, m_Color.a };

	ProtoRenderer.RenderTexture(*m_pTexture, data);
}

void Text::UpdateText()
{
	if (!m_pFont)
		return;
	
	SafeDelete(m_pTexture);
	SDL_Surface* surf{};

	if (m_Text[0] != '\0')
		surf = TTF_RenderText_Blended(m_pFont->GetFont(), m_Text.c_str(), m_Color);
	else
		surf = TTF_RenderText_Blended(m_pFont->GetFont(), " ", m_Color);

	if (!surf)
		throw std::runtime_error(std::string("Render text failed: ") + SDL_GetError());

	const auto texture = SDL_CreateTextureFromSurface(ProtoRenderer.GetSDLRenderer(), surf);
	if (!texture)
		throw std::runtime_error(std::string("Create text texture from surface failed: ") + SDL_GetError());

	SDL_FreeSurface(surf);

	m_pTexture = new Proto::Texture2D(texture, m_pFont->GetFullPath(), m_pFont->GetRelativePath());

	int width, height;
	SDL_QueryTexture(m_pTexture->GetSDLTexture(), nullptr, nullptr, &width, &height);
	m_TextureSize.x = float(width);
	m_TextureSize.y = float(height);
}

void Text::Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent)
{
	using namespace rapidxml;
	
	xml_node<>* pComp = doc.allocate_node(node_element, doc.allocate_string(ProtoConvert::ToString<Text>().c_str()));

	SaveID(doc, pComp);
	SaveActive(doc, pComp);
	
	// Font related
	ProtoSaver::XML::SaveString("FontLocation", m_FontRelPath, doc, pComp);
	
	auto fontSize{ m_pFont ? m_pFont->GetSize() : 13 };
	ProtoSaver::XML::Save<unsigned int>("FontSize", fontSize, doc, pComp);

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
	
	ProtoSaver::XML::SaveString("Text", m_Text, doc, pComp);

	pParent->append_node(pComp);
}

void Text::Load(rapidxml::xml_node<>* pComp, GameObject* pCurr)
{
	const auto id{ ProtoParser::XML::Parse<unsigned int>(pComp, "ID") };
	const auto isActive{ ProtoParser::XML::Parse<bool>(pComp, "Active") };

	const auto fontLocation{ ProtoParser::XML::ParseString(pComp, "FontLocation") };
	const auto fontSize{ ProtoParser::XML::Parse<int>(pComp, "FontSize") };

	const auto text{ ProtoParser::XML::ParseString(pComp, "Text") };

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

	Proto::Font* pFont{ fontLocation.empty() ? nullptr : ProtoContent.GetFont(fontLocation, fontSize) };
	const auto pTextComp = new Text(id, isActive, text, pFont, position, scale, pivot, color);
	pCurr->AddComponent(pTextComp);
}
