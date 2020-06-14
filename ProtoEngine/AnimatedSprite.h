#pragma once
#include "BaseBehaviour.h"
#include "Structs.h"

namespace Proto {
	class Texture2D;
}

struct Animation
{
	float frameTime{ 0.2f };
	std::vector<TextureFrame> frames{ 0 };

	void Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent);
	static Animation Load(rapidxml::xml_node<>* pComp);
};
class AnimatedSprite final : public BaseBehaviour
{
public:
	AnimatedSprite(ComponentID ID, bool isActive, int startingAnimation, Proto::Texture2D* pTexture);
	~AnimatedSprite();

	void SetTexture(const std::string& path);
	void AddAnimation(const Animation& animation);
	void PlayAnimation(unsigned int index, bool paused = false);
	void SetPaused(bool isPaused) { m_IsPaused = isPaused; }
	unsigned int GetPlayingAnimation() const { return m_PlayingAnimation; }
	
protected:
	void Draw() override;
	void Update() override;
	void UpdateUnscaled() override;
	
	void DrawInspectorTitle() override;
	void DrawInspector() override;

	void Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent) override;

	std::string m_TexRelPath;

	Proto::Texture2D* m_pTexture;
	glm::vec2 m_TextureSize{ 0, 0 };

	std::vector<Animation> m_Animations{};
	Animation* m_pCurrentAnimation{};
	unsigned int m_CurrentFrame{ 0 };
	float m_CurrentTime{ 0 };

	int m_StartingAnimation{ 0 };
	int m_PlayingAnimation{ 0 };
	bool m_IsPaused{ true };
	
	// Editor
	glm::vec2 m_SpritePreviewScale{ 1, 1 };
	float m_SpritePreviewRotation{ 0 };
	
	Proto::Texture2D* m_pTexturePreview{};
	Proto::Texture2D* m_pSpritePreview{};

	Animation* m_pCurrentPreviewAnimation{};
	unsigned int m_CurrentPreviewFrame{ 0 };
	float m_CurrentPreviewTime{ 0 };

	std::string m_SaveFileFolderStructure{};
	std::string m_SaveFileName{};

	void LoadAnimations(const std::string& filePath);
	void SaveAnimations();
	
public:
	static void Load(rapidxml::xml_node<>* pComp, GameObject* pCurr);
};
