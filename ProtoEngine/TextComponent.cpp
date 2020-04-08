#include "ProtoEnginePCH.h"
#include "TextComponent.h"


#include <SDL_ttf.h>
#include <utility>
#include "Font.h"
#include "Texture2D.h"
#include "Utils.h"

TextComponent::TextComponent(std::string text, Proto::Font* pFont, const Proto::TextureData& texData)
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
	SafeDelete(m_pFont);
}

void TextComponent::SetText(const std::string& text)
{
	m_Text = text;
	UpdateText();
}

void TextComponent::SetFontSize(unsigned size)
{
	const std::string relPath{ m_pFont->GetRelativePath() };
	SafeDelete(m_pFont);
	m_pFont = ProtoResources.LoadFont(relPath, size);
	
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
	
	SafeDelete(m_pFont);
	m_pFont = ProtoResources.LoadFont(path, size);
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
	ImGui::Text("Text Component");
}

void TextComponent::DrawInspector()
{
	std::stringstream thisAddress;
	thisAddress << this;

	std::string labelText{};

	/* Position */ {
		ImGui::Text("Position");

		/* Position X */ {
			ImGui::SameLine(100);
			ImGui::Text("X");
			ImGui::SameLine(115);

			ImGui::PushItemWidth(70);
			labelText = "##TEXT_COMP_POS_X" + thisAddress.str();
			ImGui::InputFloat(labelText.c_str(), &m_TexData.x, 0, 0, "%.1f");
			ImGui::PopItemWidth();
		}

		/* Position Y */ {
			ImGui::SameLine(200);
			ImGui::Text("Y");
			ImGui::SameLine(215);

			ImGui::PushItemWidth(70);
			labelText = "##TEXT_COMP_POS_Y" + thisAddress.str();
			ImGui::InputFloat(labelText.c_str(), &m_TexData.y, 0, 0, "%.1f");
			ImGui::PopItemWidth();
		}
	}

	/* Font Path */ {
		ImGui::Text("Font");
		ImGui::SameLine(100);

		labelText = "##TEXT_COMP_FILE_PATH" + thisAddress.str();

		m_FontRelPath.resize(150);

		ImGui::InputText(labelText.c_str(), &m_FontRelPath[0], 150);
		if (ProtoResources.FileExists(m_FontRelPath, ResourceType::FONT))
		{
			if (m_pFont)
			{
				if (m_FontRelPath != m_pFont->GetRelativePath())
					SetFont(m_FontRelPath);
			}

			else
				SetFont(m_FontRelPath);
		}
	}

	if (!m_pFont)
		return;
	
	/* Font Size */ {
		ImGui::Text("Font Size");
		ImGui::SameLine(100);

		int newSize{ int(m_pFont->GetSize()) };

		labelText = "##TEXT_COMP_FONT_SIZE" + thisAddress.str();
		ImGui::InputInt(labelText.c_str(), &newSize);

		if (newSize != int(m_pFont->GetSize()))
			SetFontSize(newSize);
	}

	/* Text */ {
		m_Text.resize(100);

		ImGui::Text("Text");
		ImGui::SameLine(100);

		std::string newText{ m_Text };
		newText.resize(m_Text.size());
		labelText = "##TEXT_COMP_TEXT" + thisAddress.str();
		ImGui::InputText(labelText.c_str(), &newText[0], 100);

		if (newText != m_Text)
		{
			SetText(newText);
		}
	}

	/* Text Color */ {
		ImGui::Text("Text Color");
		ImGui::SameLine(100);

		labelText = "##TEXT_COMP_TEXT_COLOR" + thisAddress.str();

		float colors[4] = { float(m_TexData.color.r) / 255.f, float(m_TexData.color.g) / 255.f, float(m_TexData.color.b) / 255.f, float(m_TexData.color.a) / 255.f };
		ImGui::ColorEdit4(labelText.c_str(), &colors[0]);
		const SDL_Color newColor{ Uint8(colors[0] * 255), Uint8(colors[1] * 255), Uint8(colors[2] * 255), Uint8(colors[3] * 255) };

		if (newColor.r != m_TexData.color.r || newColor.g != m_TexData.color.g || newColor.b != m_TexData.color.b || newColor.a != m_TexData.color.a)
			SetColor(newColor);
	}
}

void TextComponent::Draw()
{
	if (!m_pTexture)
		return;
	
	if (m_TexData.width == -1 || m_TexData.height == -1)
		ProtoRenderer.RenderTexture(*m_pTexture, m_pGameObject->GetTransform()->GetPosition().x + m_TexData.x, m_pGameObject->GetTransform()->GetPosition().y + m_TexData.y, { 255, 255, 255, m_TexData.color.a });

	else
		ProtoRenderer.RenderTexture(*m_pTexture, m_pGameObject->GetTransform()->GetPosition().x + m_TexData.x, m_pGameObject->GetTransform()->GetPosition().y + m_TexData.y, m_TexData.width, m_TexData.height, { 255, 255, 255, m_TexData.color.a });
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
}
