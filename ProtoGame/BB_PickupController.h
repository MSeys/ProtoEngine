#pragma once
#include "BaseBehaviour.h"
#include "BB_GameMode.h"

class BB_PickupController final : public BaseBehaviour
{
public:
	BB_PickupController(const ComponentID ID, const bool isActive, int score)
		: BaseBehaviour(ID, isActive), m_Score(score)
	{
	}

	void DrawInspectorTitle() override;
	void DrawInspector() override;
	int GetScore() const { return m_Score; }
	
protected:
	void Start() override;
	void Awake() override;

	void Save(rapidxml::xml_document<>&, rapidxml::xml_node<>*) override {}

	void OnTriggerEnter(const Collision& collision) override;
	void PreSolveCollision(const Collision& collision) override;
	
private:
	int m_Score{ 100 };
	
	BB_GameMode* m_pGameMode{};
};
