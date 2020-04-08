#pragma once
#include <string>

#include "RenderComponent.h"

namespace Proto {
	class Font;
}

class TextComponent : public BaseComponent
{
public:
	TextComponent(std::string text, Proto::Font* pFont, const Proto::TextureData& texData = { 0, 0 });
	~TextComponent() override;

	void SetText(const std::string& text);
	void SetFontSize(unsigned size);
	void SetColor(const SDL_Color& color);
	void SetFont(const std::string& path);
	
protected:
	void Initialize() override;
	void Update() override;
	void FixedUpdate() override;
	
	void DrawInspectorTitle() override;
	void DrawInspector() override;

	void Draw() override;

	void UpdateText();
	
	std::string m_FontRelPath;
	Proto::Texture2D* m_pTexture;
	Proto::TextureData m_TexData;
	std::string m_Text;
	Proto::Font* m_pFont;
};

