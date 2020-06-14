#pragma once
#include "BaseBehaviour.h"
#include "Structs.h"

namespace Proto {
	class Texture2D;
}

class Sprite final : public BaseBehaviour
{
public:
	Sprite(ComponentID ID, bool isActive, Proto::Texture2D* pTexture);
	~Sprite();
	
	void SetTexture(const std::string& path);
	void AddFrame(const TextureFrame& frame);
	void PopFrame();
	void RemoveFrame(unsigned int index);
	void ResizeFrameList(unsigned int size);
	
protected:
	void Draw() override;

	void DrawInspectorTitle() override;
	void DrawInspector() override;

	void Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent) override;

	std::string m_TexRelPath;

	Proto::Texture2D* m_pTexture;
	glm::vec2 m_TextureSize{ 0, 0 };

	glm::vec2 m_SpritePreviewScale{ 1, 1 };
	float m_SpritePreviewRotation{ 0 };
	
	std::vector<TextureFrame> m_Frames{};

	Proto::Texture2D* m_pTexturePreview{};
	Proto::Texture2D* m_pSpritePreview{};

public:
	static void Load(rapidxml::xml_node<>* pComp, GameObject* pCurr);
};