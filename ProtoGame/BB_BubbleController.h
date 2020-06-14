#pragma once
#include "BaseBehaviour.h"

class BB_BubbleController final : public BaseBehaviour
{
public:
	BB_BubbleController(const ComponentID ID, const bool isActive, bool isLookingLeft)
		: BaseBehaviour(ID, isActive), m_IsLookingLeft(isLookingLeft)
	{
	}

	void DrawInspectorTitle() override;
	void DrawInspector() override;

protected:
	void Awake() override;
	void Update() override;

	void Save(rapidxml::xml_document<>&, rapidxml::xml_node<>*) override {}
	
	void OnCollisionEnter(const Collision& collision) override;;

private:
	bool m_IsLookingLeft{ false };
	float m_BubbleHealth{ 3.f };
	const float m_BubbleInitialForce{ 10.f };
};
