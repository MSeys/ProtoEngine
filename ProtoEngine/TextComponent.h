#pragma once
#include <string>

#include "ImageComponent.h"

namespace Proto {
	class Font;
}

class TextComponent : public BaseComponent
{
public:
	TextComponent(std::string text, Proto::Font* pFont, const TextureData& texData = { 0, 0 });
	~TextComponent() override;

	void SetText(const std::string& text);
	void SetFontSize(unsigned size);
	void SetColor(const SDL_Color& color);
	void SetFont(const std::string& path);

	void SetTextureData(const TextureData& texData);
	void SetAlignment(const HAlignment& horAlignment, const VAlignment& verAlignment);
	
protected:	
	void DrawInspectorTitle() override;
	void DrawInspector() override;

	void Draw() override;

	void UpdateText();

	void Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent) override;
	
	std::string m_FontRelPath;
	Proto::Texture2D* m_pTexture;
	TextureData m_TexData;
	std::string m_Text;
	Proto::Font* m_pFont;

	HAlignment m_HorAlignment{ HAlignment::LEFT };
	VAlignment m_VerAlignment{ VAlignment::BOTTOM };
};

