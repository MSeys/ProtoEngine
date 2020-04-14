#include "ProtoEnginePCH.h"
#include "TextComponent.h"


#include <SDL_ttf.h>
#include <utility>
#include "Font.h"
#include "Texture2D.h"
#include "Utils.h"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

TextComponent::TextComponent(std::string text, Proto::Font* pFont, const TextureData& texData)
	: m_pTexture(nullptr), m_TexData(texData), m_Text(std::move(text)), m_pFont(pFont)
{
	if(pFont)
	{
		m_FontRelPath = pFont->GetRelativePath();
		UpdateText();
	}
}

TextComponent::~TextComponent()
{
	SafeDelete(m_pTexture);
}

void TextComponent::SetText(const std::string& text)
{
	m_Text = text;
	UpdateText();
}

void TextComponent::SetFontSize(unsigned size)
{
	const std::string relPath{ m_pFont->GetRelativePath() };
	m_pFont = ProtoContent.GetFont(relPath, size);
	
	UpdateText();
}

void TextComponent::SetColor(const SDL_Color& color)
{
	m_TexData.color = color;
	UpdateText();
}

void TextComponent::SetFont(const std::string& path)
{
	unsigned int size{ 25 };
	if(m_pFont)
		size = m_pFont->GetSize();
	
	m_pFont = ProtoContent.GetFont(path, size);
	m_FontRelPath = path;

	UpdateText();
}

void TextComponent::Initialize()
{
}

void TextComponent::Update()
{
}

void TextComponent::FixedUpdate()
{
}

void TextComponent::DrawInspectorTitle()
{
	ImGui::Text("Text");
}

void TextComponent::DrawInspector()
{
	std::stringstream thisAddress;
	thisAddress << this;

	std::string labelText;

	/* Font Path */ {
		ImGui::Text("Font");
		ImGui::SameLine(100);

		ImGui::PushItemWidth(175);
		labelText = "##TEXT_COMP_FILE_PATH" + thisAddress.str();
		ImGui::InputText(labelText.c_str(), &m_FontRelPath[0], 300, ImGuiInputTextFlags_ReadOnly);
		ImGui::PopItemWidth();
		
		ImGui::SameLine(285);
		labelText = "...##TEXT_COMP_FILE_PATH_BUTTON" + thisAddress.str();
		if (ImGui::Button(labelText.c_str()))
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

	ImGuiProto::Position(m_TexData, thisAddress.str());
	ImGuiProto::Size(m_TexData, thisAddress.str());
	
	/* Font Size */ {
		ImGui::Text("Font Size");
		ImGui::SameLine(100);

		int newSize{ int(m_pFont->GetSize()) };

		labelText = "##TEXT_COMP_FONT_SIZE" + thisAddress.str();
		ImGui::DragInt(labelText.c_str(), &newSize, 1, 10, 100);

		if (newSize != int(m_pFont->GetSize()))
			SetFontSize(newSize);
	}

	/* Text */ {
		m_Text.resize(200);

		ImGui::Text("Text");
		ImGui::SameLine(100);

		std::string newText{ m_Text };
		newText.resize(m_Text.size());
		labelText = "##TEXT_COMP_TEXT" + thisAddress.str();
		ImGui::InputText(labelText.c_str(), &newText[0], 200);

		if (newText != m_Text)
		{
			SetText(newText);
		}
	}

	ImGui::PushID(this);
	ImGuiProto::Alignment(m_HorAlignment, m_VerAlignment);
	ImGui::PopID();
	
	/* Text Color */ {
		SDL_Color newColor{};
		ImGuiProto::Color("Text Color", m_TexData, newColor, thisAddress.str());

		if (newColor.r != m_TexData.color.r || newColor.g != m_TexData.color.g || newColor.b != m_TexData.color.b || newColor.a != m_TexData.color.a)
			SetColor(newColor);
	}
}

void TextComponent::Draw()
{
	if (!m_pTexture)
		return;
	float x{ m_pGameObject->GetTransform()->GetPosition().x + m_TexData.x }, y{ m_pGameObject->GetTransform()->GetPosition().y + m_TexData.y };

	switch (m_HorAlignment)
	{
	case HAlignment::LEFT:
		break;
	case HAlignment::CENTER:
		x -= m_TexData.width / 2.f;
		break;
	case HAlignment::RIGHT:
		x -= m_TexData.width;
		break;
	default:
		break;
	}

	switch (m_VerAlignment)
	{
	case VAlignment::TOP:
		y -= m_TexData.height;
		break;
	case VAlignment::CENTER:
		y -= m_TexData.height / 2.f;
		break;
	case VAlignment::BOTTOM:
		break;
	default:
		break;
	}

	ProtoRenderer.RenderTexture(*m_pTexture, x, y,
		m_pGameObject->GetTransform()->GetScale().x * m_TexData.width,
		m_pGameObject->GetTransform()->GetScale().y * m_TexData.height, { 255, 255, 255, m_TexData.color.a });
}

void TextComponent::UpdateText()
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
