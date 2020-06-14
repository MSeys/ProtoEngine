#pragma once
#include "BaseBehaviour.h"
#include "RapidXML/rapidxml.hpp"

class BB_PlayerController final : public BaseBehaviour
{
public:

	BB_PlayerController(const ComponentID ID, const bool isActive, int playerID, float walkSpeed, float jumpForce)
		: BaseBehaviour(ID, isActive), m_PlayerID(playerID), m_WalkSpeed(walkSpeed), m_JumpForce(jumpForce)
	{
	}

	void DrawInspectorTitle() override;
	void DrawInspector() override;

protected:
	void Start() override;
	void Awake() override;
	void FixedUpdate() override;

	void OnCollisionEnter(const Collision& collision) override;
	void OnCollisionExit(const Collision& collision) override;
	void PreSolveCollision(const Collision& collision) override;

	void Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent) override;

private:
	friend class PlayerMoveStopCommand;
	friend class PlayerMovingCommand;
	friend class PlayerJumpCommand;
	
	int m_PlayerID{ 0 };
	glm::vec2 m_Velocity{ 0, 0 };

	float m_WalkSpeed{ 5.f };
	float m_JumpForce{ 10.f };

	RigidBody2D* m_pRigidBody{};
	AnimatedSprite* m_pAnimatedSprite{};
	BoxCollider2D* m_pBoxCollider{};

	int m_FootContacts{ 0 };
	
	void Move(float xVelocity);
	void Jump() const;
	
public:
	static void Load(rapidxml::xml_node<>* pComp, GameObject* pCurr);
};

class PlayerMovingCommand : public Command
{
public:
	void Execute() override
	{	
		auto& playerController = VoidCast<BB_PlayerController>(EData);
		if (IData.Origin == CommandOrigin::KBM_KEY)
		{
			if (IData.SDLKeycode == SDLK_a)
				playerController.Move(-1.f);
			else if (IData.SDLKeycode == SDLK_d)
				playerController.Move(1.f);
		}

		else if(IData.Origin == CommandOrigin::C_JOYSTICK)
		{
			const auto& joystick = VoidCast<PositionData>(IData.Data);
			if(joystick.x < 0)
				playerController.Move(-1.f);
			else
				playerController.Move(1.f);
		}
	}
};

class PlayerMoveStopCommand : public Command
{
public:
	void Execute() override
	{
		auto& playerController = VoidCast<BB_PlayerController>(EData);
		playerController.Move(0.f);
	}
};

class PlayerJumpCommand : public Command
{
public:
	void Execute() override
	{
		auto& playerController = VoidCast<BB_PlayerController>(EData);
		playerController.Jump();
	}
};