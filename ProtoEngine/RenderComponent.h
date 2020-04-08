#pragma once
#include "BaseComponent.h"
#include "Structs.h"

class RenderComponent : public BaseComponent
{
public:
	RenderComponent(Proto::Texture2D* pTexture, const Proto::TextureData& texData);
	~RenderComponent();

	void SetTexture(const std::string& path);
	void SetTextureData(const Proto::TextureData& texData);
	
protected:
	void Initialize() override;
	void Update() override;
	void FixedUpdate() override;
	
	void Draw() override;

	void DrawInspectorTitle() override;
	void DrawInspector() override;

	std::string m_TexRelPath;
	Proto::Texture2D* m_pTexture;
	Proto::TextureData m_TexData;
};