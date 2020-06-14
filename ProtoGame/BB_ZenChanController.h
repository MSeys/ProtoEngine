#pragma once
#include "BaseBehaviour.h"
#include "BB_GameMode.h"
#include "RapidXML/rapidxml.hpp"

class BB_ZenChanController  final : public BaseBehaviour
{
public:
	BB_ZenChanController(const ComponentID ID, const bool isActive, float walkSpeed = 2.f, float jumpForce = -4.5f, float jumpCooldown = 0.2f, float stuckInBubbleCooldown = 1.5f)
		: BaseBehaviour(ID, isActive), m_WalkSpeed(walkSpeed), m_JumpForce(jumpForce), m_JumpCooldown(jumpCooldown), m_StuckInBubbleCooldown(stuckInBubbleCooldown)
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
	
	float m_JumpCooldown{ 0.2f };
	float m_JumpCooldownTimer{ 0.f };
	bool m_CanJump{ true };

	float m_StuckInBubbleCooldown{ 1.5f };
	float m_StuckInBubbleCooldownTimer{ 0.f };
	bool m_StuckInBubble{ false };

	bool m_WasHit{ false };
	
	glm::vec2 m_Velocity{ 0, 0 };
	
	RigidBody2D* m_pRigidBody{};
	AnimatedSprite* m_pAnimatedSprite{};
	BoxCollider2D* m_pBoxCollider{};

	BB_GameMode* m_pGameMode{};
	
	int m_FootContacts{ 0 };
	
	void Move();
	void Jump();
	
public:
	static void Load(rapidxml::xml_node<>* pComp, GameObject* pCurr);
};

