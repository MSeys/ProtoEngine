#pragma once
#include "BaseComponent.h"
#include "Structs.h"

class ImageComponent : public BaseComponent
{
public:
	ImageComponent(Proto::Texture2D* pTexture, const TextureData& texData);
	~ImageComponent();

	void SetTexture(const std::string& path);
	void SetTextureData(const TextureData& texData);
	
protected:
	void Initialize() override;
	void Update() override;
	void FixedUpdate() override;
	
	void Draw() override;

	void DrawInspectorTitle() override;
	void DrawInspector() override;

	std::string m_TexRelPath;
	Proto::Texture2D* m_pTexture;
	TextureData m_TexData;

	HAlignment m_HorAlignment{ HAlignment::LEFT };
	VAlignment m_VerAlignment{ VAlignment::BOTTOM };
};