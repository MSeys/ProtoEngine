#pragma once
#include "BaseBehaviour.h"
#include "Structs.h"

namespace Proto {
	class Texture2D;
}

class Image final : public BaseBehaviour
{
public:
	Image(ComponentID ID, bool isActive, Proto::Texture2D* pTexture, const TextureData& texData);
	~Image();

	void SetTexture(const std::string& path);
	void SetTextureData(const TextureData& texData);

	void SetAlignment(const HAlignment& horAlignment, const VAlignment& verAlignment);
	
protected:
	void Draw() override;

	void DrawInspectorTitle() override;
	void DrawInspector() override;

	void Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent) override;
	
	std::string m_TexRelPath;
	Proto::Texture2D* m_pTexture;
	TextureData m_TexData;

	HAlignment m_HorAlignment{ HAlignment::LEFT };
	VAlignment m_VerAlignment{ VAlignment::BOTTOM };
};
