#pragma once
#include "BaseComponent.h"
#include "Structs.h"

class RenderComponent : public BaseComponent
{
public:
	RenderComponent(Proto::Texture2D* pTexture, const Proto::TextureData& texData);
	~RenderComponent();

	void SetTextureData(const Proto::TextureData& texData);
	
protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;

	Proto::Texture2D* m_pTexture;
	Proto::TextureData m_TexData;
};
