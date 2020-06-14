#pragma once
#include "BaseBehaviour.h"
#include "BB_GameMode.h"
#include "RapidXML/rapidxml.hpp"

class BB_ZenChanController  final : public BaseBehaviour
{
public:
	BB_ZenChanController(const ComponentID ID, const bool isActive, float walkSpeed, float jumpForce)
		: BaseBehaviour(ID, isActive), m_WalkSpeed(walkSpeed), m_JumpForce(jumpForce)
	{
	}

	void DrawInspector() override;
	void DrawInspectorTitle() override;
	
protected:
	void Start() override;
	void Awake() override;
	void Update() override;
	void FixedUpdate() override;
	void Draw() override;

	void OnCollisionEnter(const Collision& collision) override;
	void OnCollisionExit(const Collision& collision) override;
	void PreSolveCollision(const Collision& collision) override;

	void Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent) override;

private:
	float m_WalkSpeed{ 5.f };
	float m_JumpForce{ 10.f };

	glm::vec2 m_Velocity{ 0, 0 };
	
	RigidBody2D* m_pRigidBody{};
	AnimatedSprite* m_pAnimatedSprite{};
	BoxCollider2D* m_pBoxCollider{};

	BB_GameMode* m_pGameMode{};
	
	int m_FootContacts{ 0 };
	
public:
	static void Load(rapidxml::xml_node<>* pComp, GameObject* pCurr);
};

