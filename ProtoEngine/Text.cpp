#include "ProtoEnginePCH.h"
#include "Text.h"


#include <SDL_ttf.h>
#include <utility>
#include "Font.h"
#include "Texture2D.h"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

Text::Text(ComponentID ID, bool isActive, std::string text, Proto::Font* pFont, const TextureData& texData)
	: BaseBehaviour(ID, isActive), m_pTexture(nullptr), m_TexData(texData), m_Text(std::move(text)), m_pFont(pFont)
{
	if(pFont)
	{
		m_FontRelPath = pFont->GetRelativePath();
		UpdateText();
	}
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

void Text::SetColor(const SDL_Color& color)
{
	m_TexData.color = color;
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

void Text::SetTextureData(const TextureData& texData)
{
	m_TexData = texData;
}

void Text::SetAlignment(const HAlignment& horAlignment, const VAlignment& verAlignment)
{
	m_HorAlignment = horAlignment;
	m_VerAlignment = verAlignment;
}

void Text::DrawInspectorTitle()
{
	DrawActiveCheckbox();
	ImGui::Text("Text");
}

void Text::DrawInspector()
{
	/* Font Path */ {
		ImGui::Text("Font");
		ImGui::SameLine(100);

		ImGui::PushItemWidth(175);
		ImGui::InputText("##TEXT_COMP_FILE_PATH", &m_FontRelPath[0], 300, ImGuiInputTextFlags_ReadOnly);
		ImGui::PopItemWidth();
		
		ImGui::SameLine(285);
		if (ImGui::Button("...##TEXT_COMP_FILE_PATH_BUTTON"))
		{
			const std::string fullPath = fs::canonical(ProtoContent.GetDataPath()).string();
			const auto selection = pfd::open_file("Select a Font", fullPath, { "Font Files", "*.ttf *.otf" }).result();
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
		}
	}

	if (!m_pFont)
		return;

	ProtoGui::Presets::Position(m_TexData.x, m_TexData.y);
	ProtoGui::Presets::Size(m_TexData.width, m_TexData.height);
	
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
		
		ImGui::PushItemWidth(250);
		ImGui::InputText("##TEXT_COMP_TEXT", &newText[0], 200);
		ImGui::PopItemWidth();

		if (newText != m_Text)
		{
			SetText(newText);
		}
	}

	ImGui::PushID(this);
	ProtoGui::Presets::Alignment(m_HorAlignment, m_VerAlignment);
	ImGui::PopID();
	
	/* Text Color */ {
		SDL_Color newColor{};
		ProtoGui::Presets::Color("Text Color", m_TexData, newColor);

		if (newColor.r != m_TexData.color.r || newColor.g != m_TexData.color.g || newColor.b != m_TexData.color.b || newColor.a != m_TexData.color.a)
			SetColor(newColor);
	}
}

void Text::Draw()
{
	if (!m_pTexture)
		return;

	RenderData data;
	data.position.x = m_pGameObject->GetTransform()->GetPosition().x + m_TexData.x;
	data.position.y = m_pGameObject->GetTransform()->GetPosition().y + m_TexData.y;
	data.size.x = m_pGameObject->GetTransform()->GetScale().x * m_TexData.width;
	data.size.y = m_pGameObject->GetTransform()->GetScale().y * m_TexData.height;
	data.rotationCenter = m_pGameObject->GetTransform()->GetRotCenter();
	data.angle = m_pGameObject->GetTransform()->GetRotAngle();
	data.color = { 255, 255, 255, m_TexData.color.a };
	
	switch (m_HorAlignment)
	{
	case HAlignment::LEFT:
		break;
	case HAlignment::CENTER:
		data.position.x -= m_TexData.width / 2.f;
		break;
	case HAlignment::RIGHT:
		data.position.x -= m_TexData.width;
		break;
	default:
		break;
	}

	switch (m_VerAlignment)
	{
	case VAlignment::TOP:
		data.position.y -= m_TexData.height;
		break;
	case VAlignment::CENTER:
		data.position.y -= m_TexData.height / 2.f;
		break;
	case VAlignment::BOTTOM:
		break;
	default:
		break;
	}

	ProtoRenderer.RenderTexture(*m_pTexture, data);
}

void Text::UpdateText()
{
	if (!m_pFont)
		return;
	
	SafeDelete(m_pTexture);
	SDL_Surface* surf{};

	if (m_Text[0] != '\0')
		surf = TTF_RenderText_Blended(m_pFont->GetFont(), m_Text.c_str(), m_TexData.color);
	else
		surf = TTF_RenderText_Blended(m_pFont->GetFont(), " ", m_TexData.color);

	if (!surf)
		throw std::runtime_error(std::string("Render text failed: ") + SDL_GetError());

	const auto texture = SDL_CreateTextureFromSurface(ProtoRenderer.GetSDLRenderer(), surf);
	if (!texture)
		throw std::runtime_error(std::string("Create text texture from surface failed: ") + SDL_GetError());

	SDL_FreeSurface(surf);

	m_pTexture = new Proto::Texture2D(texture, m_pFont->GetFullPath(), m_pFont->GetRelativePath());

	int width, height;
	SDL_QueryTexture(m_pTexture->GetSDLTexture(), nullptr, nullptr, &width, &height);
	m_TexData.width = float(width);
	m_TexData.height = float(height);
}

void Text::Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent)
{
	using namespace rapidxml;
	
	xml_node<>* pComp = doc.allocate_node(node_element, "TextBehaviour");

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

	// Text
	ProtoSaver::XML::SaveString("Text", m_Text, doc, pComp);

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
