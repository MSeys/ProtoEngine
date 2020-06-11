#pragma once
#include <list>

#include "BaseBehaviour.h"
#include "Structs.h"

namespace Proto {
	class Texture2D;
}

class Image final : public BaseBehaviour
{
public:
	Image(ComponentID ID, bool isActive, Proto::Texture2D* pTexture, const glm::vec2& position = { 0, 0 }, const glm::vec2& scale = { 1, 1 }, const glm::vec2& pivot = { 0, 0 }, const SDL_Color& color = { 255, 255, 255, 255 });

	void SetTexture(const std::string& path);
	
	void SetPosition(const glm::vec2& position) { m_Position = position; }
	void SetScale(const glm::vec2& scale) { m_Position = scale; }
	void SetPivot(const glm::vec2& pivot) { m_Pivot = pivot; }
	void SetColor(const SDL_Color& color) { m_Color = color; }
	
protected:
	void Draw() override;
	
	void DrawInspectorTitle() override;
	void DrawInspector() override;

	void Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent) override;
	
	std::string m_TexRelPath;
	
	Proto::Texture2D* m_pTexture;
	glm::vec2 m_TextureSize{ 0, 0 };
	
	glm::vec2 m_Position{ 0, 0 };
	glm::vec2 m_Scale{ 1, 1 };
	glm::vec2 m_Pivot{ 0, 0 };
	SDL_Color m_Color{ 255, 255, 255, 255 };

public:
	static void Load(rapidxml::xml_node<>* pComp, GameObject* pCurr);
};