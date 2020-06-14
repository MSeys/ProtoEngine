#pragma once
#include "BaseBehaviour.h"

class BB_PlayerController;

class BB_GameMode final : public BaseBehaviour
{
public:
	BB_GameMode(const ComponentID ID, const bool isActive, int playerOneID, int playerTwoID)
		: BaseBehaviour(ID, isActive), m_PlayerOneID(playerOneID), m_PlayerTwoID(playerTwoID)
	{
	}

	void DrawInspectorTitle() override;
	void DrawInspector() override;

	BB_PlayerController* GetPlayerOne() const { return m_pPlayerOneController; }
	BB_PlayerController* GetPlayerTwo() const { return m_pPlayerTwoController; }

	bool HasPlayerOne() const { return m_pPlayerOneController != nullptr; }
	bool HasPlayerTwo() const { return m_pPlayerTwoController != nullptr; }

	void SetBackgroundFade(float fade) const;
	
protected:
	void Start() override;
	void Awake() override;
	void Update() override;
	void FixedUpdate() override;
	void Draw() override;

	void Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent) override;
	
private:
	BB_PlayerController* m_pPlayerOneController{};
	BB_PlayerController* m_pPlayerTwoController{};
	GameObject* m_pPlayerOneUI{};
	GameObject* m_pPlayerTwoUI{};

	Sprite* m_pPlayerOneHealth{};
	Sprite* m_pPlayerTwoHealth{};

	Image* m_pFadeBackground{};

	const float m_TransitionTimer{ 2.f };
	float m_TransitionCurrTimer{ 0.f };

	int m_PlayerOneID{ -1 }, m_PlayerTwoID{ -1 };

	std::array<GameObject*, 3> m_pLevels;
	int m_CurrentLevel{ 0 };

	bool m_IsTransitioning{ false };
	bool m_IsTransitioningHalfway{ false };

public:
	static void Load(rapidxml::xml_node<>* pComp, GameObject* pCurr);
};