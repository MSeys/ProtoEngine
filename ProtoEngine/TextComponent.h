#pragma once
#include <string>

#include "RenderComponent.h"

namespace Proto {
	class Font;
}

class TextComponent : public RenderComponent
{
public:
	TextComponent(std::string text, Proto::Font* pFont, const Proto::TextureData& texData = { 0, 0 });
	~TextComponent() override;

	void SetText(const std::string& text);
	void SetFontSize(unsigned int size);
	
protected:
	void Initialize() override;
	void Update() override;

	std::string m_Text;
	Proto::Font* m_pFont;

private:
	bool m_NeedsUpdate;
};

