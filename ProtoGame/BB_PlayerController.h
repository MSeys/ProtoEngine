#pragma once
#include "BaseBehaviour.h"
#include "BB_GameMode.h"
#include "RapidXML/rapidxml.hpp"

class BB_PlayerController final : public BaseBehaviour
{
public:

	BB_PlayerController(const ComponentID ID, const bool isActive, int playerID = 0, float walkSpeed = 2.f, float jumpForce = -4.5f, float jumpCooldown = 0.2f, float spawnBubbleCooldown = 0.2f)
		: BaseBehaviour(ID, isActive), m_PlayerID(playerID), m_WalkSpeed(walkSpeed), m_JumpForce(jumpForce), m_JumpCooldown(jumpCooldown), m_SpawnBubbleCooldown(spawnBubbleCooldown)
	{
	}

	void DrawInspectorTitle() override;
	void DrawInspector() override;

	void Damage();
	bool WasHit() const { return m_WasHit; }
	int GetHealth() const { return m_Health; }
	void AddScore(int score) { m_Score += score; }
	int GetScore() const { return m_Score; }

	bool IsDead() const { return m_IsDead; }
	void ResetPosition() const;
	
protected:
	void Start() override;
	void Awake() override;
	void Update() override;
	void FixedUpdate() override;

	void OnCollisionEnter(const Collision& collision) override;
	void OnCollisionExit(const Collision& collision) override;
	void OnTriggerEnter(const Collision& collision) override;
	
	void PreSolveCollision(const Collision& collision) override;
	void PreSolveTrigger(const Collision& collision) override;
	
	void Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent) override;

private:
	friend class PlayerMoveStopCommand;
	friend class PlayerMovingCommand;
	friend class PlayerJumpCommand;
	friend class PlayerSpawnBubbleCommand;
	
	int m_PlayerID{ 0 };
	glm::vec2 m_StartingPosition{};
	glm::vec2 m_Velocity{ 0, 0 };
	bool m_IsLookingLeft{ true };

	float m_WalkSpeed{ 5.f };
	float m_JumpForce{ 10.f };

	// Health + Hit
	int m_Health{ 4 };
	bool m_WasHit{ false };
	float m_HitCooldown{ 1.f };
	float m_HitCooldownTimer{ 0.f };

	int m_Score{ 0 };
	
	bool m_IsDead{ false };
	
	// Jump
	float m_JumpCooldown{ 0.2f };
	float m_JumpCooldownTimer{ 0.f };
	bool m_CanJump{ true };

	// Spawn Bubble
	float m_SpawnBubbleCooldown{ 0.5f };
	float m_SpawnBubbleCooldownTimer{ 0.f };
	bool m_CanSpawnBubble{ true };

	BB_GameMode* m_pGameMode{};
	RigidBody2D* m_pRigidBody{};
	AnimatedSprite* m_pAnimatedSprite{};
	BoxCollider2D* m_pBoxCollider{};

	int m_FootContacts{ 0 };
	
	void Move(float xVelocity);
	void Jump();
	void SpawnBubble();
	
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

class PlayerSpawnBubbleCommand : public Command
{
public:
	void Execute() override
	{
		auto& playerController = VoidCast<BB_PlayerController>(EData);
		playerController.SpawnBubble();
	}
};