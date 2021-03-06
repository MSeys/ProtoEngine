#pragma once
#include "BaseBehaviour.h"
#include <string>

namespace Proto {
	class Font;
}

class Text : public BaseBehaviour
{
public:
	Text(ComponentID ID, bool isActive, std::string text, Proto::Font* pFont, const glm::vec2& position = { 0, 0 }, const glm::vec2& scale = { 1, 1 }, const glm::vec2& pivot = { 0, 0 }, SDL_Color color = { 255, 255, 255, 255 });
	~Text() override;

	void SetText(const std::string& text);
	void SetFontSize(unsigned size);
	void SetFont(const std::string& path);

	void SetPosition(const glm::vec2& position) { m_Position = position; }
	void SetScale(const glm::vec2& scale) { m_Position = scale; }
	void SetPivot(const glm::vec2& pivot) { m_Pivot = pivot; }
	void SetColor(const SDL_Color& color)
	{
		m_Color = color;
		UpdateText();
	}
	
protected:	
	void DrawInspectorTitle() override;
	void DrawInspector() override;

	void Draw() override;

	void UpdateText();

	void Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent) override;
	
	std::string m_FontRelPath;
	
	Proto::Texture2D* m_pTexture;
	Proto::Font* m_pFont;

	std::string m_Text;
	glm::vec2 m_TextureSize{ 0, 0 };

	glm::vec2 m_Position{ 0, 0 };
	glm::vec2 m_Scale{ 1, 1 };
	glm::vec2 m_Pivot{ 0, 0 };
	SDL_Color m_Color{ 255, 255, 255, 255 };

public:
	static void Load(rapidxml::xml_node<>* pComp, GameObject* pCurr);
};

