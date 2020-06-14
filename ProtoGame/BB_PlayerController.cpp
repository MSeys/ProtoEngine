#include "ProtoEnginePCH.h"
#include "BB_PlayerController.h"

#include "BB_BubbleController.h"
#include "BB_GameMode.h"

void BB_PlayerController::DrawInspectorTitle()
{
	ImGui::Text("BB - Player Controller");
}

void BB_PlayerController::DrawInspector()
{
	const ProtoGui::ProtoGuiData pgData{ true, 120, -1, true, 70 };
	ProtoGui::DragData dragData{ 1.f, 0, 1, "%.0f" };
	ProtoGui::Drag<int>("Player ID", pgData, "##BB_PlayerController_PlayerID", m_PlayerID, dragData);

	dragData.v_min = dragData.v_max = 0;
	dragData.v_speed = 0.1f;
	dragData.format = "%0.1f";
	ProtoGui::Drag<float>("Walk Speed", pgData, "##BB_PlayerController_WalkSpeed", m_WalkSpeed, dragData);
	ProtoGui::Drag<float>("Jump Force", pgData, "##BB_PlayerController_JumpForce", m_JumpForce, dragData);
	ProtoGui::Drag<float>("Jump Cooldown", pgData, "##BB_PlayerController_JumpCooldown", m_JumpCooldown, dragData);
	ProtoGui::Drag<float>("Bubble Cooldown", pgData, "##BB_PlayerController_BubbleCooldown", m_SpawnBubbleCooldown, dragData);
}

void BB_PlayerController::ResetPosition() const
{
	m_pRigidBody->GetBody()->SetTransform(ProtoConvert::ToBox2DVec(ProtoConvert::PixelsToBox2D(m_StartingPosition)), 0);
}

void BB_PlayerController::Start()
{
}

void BB_PlayerController::Awake()
{
	GameObject* pGameModeObject{ Gameobject.GetScene()->FindGameObjectInRootWithName("GameMode") };
	if (pGameModeObject)
		m_pGameMode = pGameModeObject->GetComponent<BB_GameMode>();
	
	m_StartingPosition = GetTransform()->GetPosition();
	
	const std::string moveID{ "BubbleBobble_Player_Move_" + std::to_string(m_PlayerID) },
					  moveStopID{ "BubbleBobble_Player_MoveStop_" + std::to_string(m_PlayerID) },
					  jumpID{ "BubbleBobble_Player_Jump_" + std::to_string(m_PlayerID) },
					  spawnBubbleID{ "BubbleBobble_Player_SpawnBubble_" + std::to_string(m_PlayerID) };
	
	m_pRigidBody = Gameobject.GetComponent<RigidBody2D>();
	m_pAnimatedSprite = Gameobject.GetComponent<AnimatedSprite>();
	m_pBoxCollider = Gameobject.GetComponent<BoxCollider2D>();
	
	ProtoCommands.AddCommand(new PlayerMovingCommand(), moveID);
	ProtoCommands.AddCommand(new PlayerMoveStopCommand(), moveStopID);
	ProtoCommands.AddCommand(new PlayerJumpCommand(), jumpID);
	ProtoCommands.AddCommand(new PlayerSpawnBubbleCommand(), spawnBubbleID);

	ProtoCommands.GetCommand(moveID).SetExecuteData(this);
	ProtoCommands.GetCommand(moveStopID).SetExecuteData(this);
	ProtoCommands.GetCommand(jumpID).SetExecuteData(this);
	ProtoCommands.GetCommand(spawnBubbleID).SetExecuteData(this);
	
	if (m_PlayerID == 0)
	{
		ProtoInput.AddInput(SDLK_a);
		ProtoInput.GetInput(SDLK_a).SetCommand(ButtonState::Held, moveID);
		ProtoInput.GetInput(SDLK_a).SetCommand(ButtonState::Released, moveStopID);

		ProtoInput.AddInput(SDLK_d);
		ProtoInput.GetInput(SDLK_d).SetCommand(ButtonState::Held, moveID);
		ProtoInput.GetInput(SDLK_d).SetCommand(ButtonState::Released, moveStopID);

		ProtoInput.AddInput(SDLK_w);
		ProtoInput.GetInput(SDLK_w).SetCommand(ButtonState::Pressed, jumpID);

		ProtoInput.AddInput(SDLK_s);
		ProtoInput.GetInput(SDLK_s).SetCommand(ButtonState::Pressed, spawnBubbleID);
		m_IsLookingLeft = false;
	}

	else
	{
		ProtoInput.AddInput(SDLK_LEFT);
		ProtoInput.GetInput(SDLK_LEFT).SetCommand(ButtonState::Held, moveID);
		ProtoInput.GetInput(SDLK_LEFT).SetCommand(ButtonState::Released, moveStopID);

		ProtoInput.AddInput(SDLK_RIGHT);
		ProtoInput.GetInput(SDLK_RIGHT).SetCommand(ButtonState::Held, moveID);
		ProtoInput.GetInput(SDLK_RIGHT).SetCommand(ButtonState::Released, moveStopID);

		ProtoInput.AddInput(SDLK_UP);
		ProtoInput.GetInput(SDLK_UP).SetCommand(ButtonState::Pressed, jumpID);

		ProtoInput.AddInput(SDLK_DOWN);
		ProtoInput.GetInput(SDLK_DOWN).SetCommand(ButtonState::Pressed, spawnBubbleID);
	}

	ProtoInput.GetInput(m_PlayerID, StickState::Left).SetCommand(JoystickState::Moving, moveID);
	ProtoInput.GetInput(m_PlayerID, StickState::Left).SetCommand(JoystickState::Released, moveStopID);

	ProtoInput.AddInput(m_PlayerID, XINPUT_GAMEPAD_LEFT_SHOULDER);
	ProtoInput.GetInput(m_PlayerID, XINPUT_GAMEPAD_LEFT_SHOULDER).SetCommand(ButtonState::Pressed, jumpID);

	ProtoInput.AddInput(m_PlayerID, XINPUT_GAMEPAD_RIGHT_SHOULDER);
	ProtoInput.GetInput(m_PlayerID, XINPUT_GAMEPAD_RIGHT_SHOULDER).SetCommand(ButtonState::Pressed, spawnBubbleID);
}

void BB_PlayerController::Update()
{
	if (!m_CanJump)
	{
		m_JumpCooldownTimer += ProtoTime.DeltaTime;
		if (m_JumpCooldownTimer > m_JumpCooldown)
		{
			m_CanJump = true;
			m_JumpCooldownTimer = 0.f;
		}
	}

	if (!m_CanSpawnBubble)
	{
		m_SpawnBubbleCooldownTimer += ProtoTime.DeltaTime;
		if (m_SpawnBubbleCooldownTimer > m_SpawnBubbleCooldown)
		{
			m_CanSpawnBubble = true;
			m_SpawnBubbleCooldownTimer = 0.f;
		}
	}
}

void BB_PlayerController::FixedUpdate()
{
	if (m_WasHit)
	{
		if(m_IsDead)
			m_pGameMode->SetBackgroundFade(m_HitCooldownTimer / m_HitCooldown);
		
		m_HitCooldownTimer += ProtoTime.DeltaTime;
		if (m_HitCooldownTimer > m_HitCooldown)
		{
			if (m_IsDead)
			{
				ProtoScenes.Load(L"BubbleBobble_Menu");
				return;
			}
			
			m_WasHit = false;
			m_HitCooldownTimer = 0.f;
			m_Velocity = { 0, 0 };
			m_pRigidBody->GetBody()->SetGravityScale(1.f);
			m_pRigidBody->GetBody()->SetTransform(ProtoConvert::ToBox2DVec(ProtoConvert::PixelsToBox2D(m_StartingPosition)), 0);

			if(m_PlayerID == 0)
			{
				m_IsLookingLeft = false;
				m_pAnimatedSprite->PlayAnimation(1);
			}

			else
			{
				m_IsLookingLeft = true;
				m_pAnimatedSprite->PlayAnimation(0);
			}
		}

		return;
	}
	
	const float velChange = m_Velocity.x * m_WalkSpeed - m_pRigidBody->GetBody()->GetLinearVelocity().x;
	const float impulse = m_pRigidBody->GetBody()->GetMass() * velChange;

	// Found about Linear Impulse at https://www.iforce2d.net/b2dtut/jumping
	m_pRigidBody->GetBody()->ApplyLinearImpulse({ impulse, 0 }, m_pRigidBody->GetBody()->GetWorldCenter(), true);

	if (GetTransform()->GetPosition().y > ProtoSettings.GetWindowSize().y + 50.f)
		m_pRigidBody->GetBody()->SetTransform(ProtoConvert::ToBox2DVec(ProtoConvert::PixelsToBox2D({ GetTransform()->GetPosition().x, -10.f })), 0);
}

void BB_PlayerController::Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent)
{
	using namespace rapidxml;

	xml_node<>* pComp = doc.allocate_node(node_element, doc.allocate_string(ProtoConvert::ToString<BB_PlayerController>().c_str()));
	SaveID(doc, pComp);
	SaveActive(doc, pComp);

	ProtoSaver::XML::Save<int>("PlayerID", m_PlayerID, doc, pComp);
	ProtoSaver::XML::Save<float>("WalkSpeed", m_WalkSpeed, doc, pComp);
	ProtoSaver::XML::Save<float>("JumpForce", m_JumpForce, doc, pComp);
	ProtoSaver::XML::Save<float>("JumpCooldown", m_JumpCooldown, doc, pComp);
	ProtoSaver::XML::Save<float>("BubbleCooldown", m_SpawnBubbleCooldown, doc, pComp);
	
	pParent->append_node(pComp);
}

void BB_PlayerController::Move(float xVelocity)
{
	if (WasHit())
		return;

	if (m_pGameMode)
	{
		if (m_pGameMode->GetPlayerOne()->IsDead())
			return;

		if (m_pGameMode->HasPlayerTwo())
		{
			if (m_pGameMode->GetPlayerTwo()->IsDead())
				return;
		}
	}
	
	if (m_Velocity.x != -1 && xVelocity == -1)
	{
		m_IsLookingLeft = true;
		m_pAnimatedSprite->PlayAnimation(1);
	}

	else if (m_Velocity.x != 1 && xVelocity == 1)
	{
		m_IsLookingLeft = false;
		m_pAnimatedSprite->PlayAnimation(0);
	}

	m_Velocity.x = xVelocity;
}

void BB_PlayerController::Jump()
{
	if (WasHit())
		return;

	if (m_pGameMode)
	{
		if (m_pGameMode->GetPlayerOne()->IsDead())
			return;

		if (m_pGameMode->HasPlayerTwo())
		{
			if (m_pGameMode->GetPlayerTwo()->IsDead())
				return;
		}
	}

	if (m_FootContacts > 0 && m_CanJump)
	{
		m_pRigidBody->GetBody()->ApplyLinearImpulse({ 0, m_pRigidBody->GetBody()->GetMass() * m_JumpForce }, m_pRigidBody->GetBody()->GetWorldCenter(), true);
		m_CanJump = false;
	}
}

void BB_PlayerController::SpawnBubble()
{
	if (WasHit())
		return;

	if (m_pGameMode)
	{
		if (m_pGameMode->GetPlayerOne()->IsDead())
			return;

		if (m_pGameMode->HasPlayerTwo())
		{
			if (m_pGameMode->GetPlayerTwo()->IsDead())
				return;
		}
	}

	if (!m_CanSpawnBubble)
		return;
	
	GameObject* pBubble{ new GameObject(Gameobject.GetScene()->RequestNewID(), "Bubble", true) };
	Gameobject.GetScene()->AddChild(pBubble, true);
	pBubble->GetTransform()->SetPosition(GetTransform()->GetPosition().x, GetTransform()->GetPosition().y - 15.f);
	
	Sprite* pBubbleSprite{ new Sprite(pBubble->RequestNewID(), true, ProtoContent.GetTexture("\\BubbleBobble\\bubbles.png")) };
	pBubble->AddComponent(pBubbleSprite, true);
	
	TextureFrame frame;
	frame.dstPosition = { 0, 0 };
	frame.dstSize = { 16, 16 };
	frame.pivot = { 0.5f, 0.5f };
	frame.scale = { 2, 2 };
	if (m_PlayerID == 0)
		frame.srcPosition = { 0, 0 };
	else
		frame.srcPosition = { 0, 16 };
	
	frame.srcSize = { 16, 16 };
	frame.color = { 255, 255, 255, 255 };
	pBubbleSprite->AddFrame(frame);

	pBubble->AddComponent(new RigidBody2D(pBubble->RequestNewID(), RigidBodyType::DYNAMIC, -0.02f, true, true), true);
	pBubble->AddComponent(new SphereCollider2D(pBubble->RequestNewID(), true, {}, 15.f, 1.f, 0.f, 0.2f, false), true);
	pBubble->AddComponent(new BB_BubbleController(pBubble->RequestNewID(), true, m_IsLookingLeft), true);

	m_CanSpawnBubble = false;
	
}

void BB_PlayerController::Damage()
{
	if (m_pGameMode)
	{
		if (m_pGameMode->GetPlayerOne()->IsDead())
			return;

		if (m_pGameMode->HasPlayerTwo())
		{
			if (m_pGameMode->GetPlayerTwo()->IsDead())
				return;
		}
	}

	m_Health--;
	m_WasHit = true;
	m_IsDead = m_Health == 0;
	m_pAnimatedSprite->PlayAnimation(2);
	m_pRigidBody->GetBody()->SetGravityScale(-0.02f);
}

void BB_PlayerController::Load(rapidxml::xml_node<>* pComp, GameObject* pCurr)
{
	const auto id{ ProtoParser::XML::Parse<unsigned int>(pComp, "ID") };
	const auto isActive{ ProtoParser::XML::Parse<bool>(pComp, "Active") };

	const auto playerID{ ProtoParser::XML::Parse<int>(pComp, "PlayerID") };
	const auto walkSpeed{ ProtoParser::XML::Parse<float>(pComp, "WalkSpeed") };
	const auto jumpForce{ ProtoParser::XML::Parse<float>(pComp, "JumpForce") };
	const auto jumpCooldown{ ProtoParser::XML::Parse<float>(pComp, "JumpCooldown") };
	const auto bubbleCooldown{ ProtoParser::XML::Parse<float>(pComp, "BubbleCooldown") };
	
	pCurr->AddComponent(new BB_PlayerController(id, isActive, playerID, walkSpeed, jumpForce, jumpCooldown, bubbleCooldown));
}

void BB_PlayerController::OnCollisionEnter(const Collision& collision)
{
	if (m_WasHit)
		return;
	
	if (collision.other->GetName() == "Walls")
		return;

	m_FootContacts++;
}

void BB_PlayerController::OnCollisionExit(const Collision& collision)
{
	if (m_WasHit)
		return;
	
	if (collision.other->GetName() == "Walls")
		return;
	
	m_FootContacts--;
}

void BB_PlayerController::PreSolveCollision(const Collision& collision)
{
	b2Fixture* fixtureA{ collision.contact->GetFixtureA() }, *fixtureB{ collision.contact->GetFixtureB() };	
	b2Fixture* playerFixture{ static_cast<GameObject*>(fixtureA->GetBody()->GetUserData()) == GetGameObject() ? fixtureA : fixtureB };

	if(m_WasHit)
	{
		if(collision.other->GetName() == "Walls" || collision.other->GetName() == "Floors")
			collision.contact->SetEnabled(true);
		else
			collision.contact->SetEnabled(false);

		return;
	}
	
	if (collision.other->GetName() == "Bubble")
	{
		collision.contact->SetEnabled(false);
		return;
	}

	if (collision.other->GetName() == "Bubble")
	{
		collision.contact->SetEnabled(false);
		return;
	}

	if (collision.other->GetName() == "Floors")
	{
		// Derived from https://www.iforce2d.net/b2dtut/one-way-walls
		b2Body* playerBody = playerFixture->GetBody();

		const int numPoints = collision.contact->GetManifold()->pointCount;
		b2WorldManifold worldManifold;
		collision.contact->GetWorldManifold(&worldManifold);

		//check if contact points are moving downward
		for (int i = 0; i < numPoints; i++)
		{
			const b2Vec2 pointVel = playerBody->GetLinearVelocityFromWorldPoint(worldManifold.points[i]);
			if (pointVel.y >= 0)
				return;//point is moving down, leave contact solid and exit
		}

		//no points are moving downward, contact should not be solid
		collision.contact->SetEnabled(false);
	}
}

void BB_PlayerController::PreSolveTrigger(const Collision& collision)
{
	if (m_WasHit)
	{
		collision.contact->SetEnabled(false);
		return;
	}

	// TODO: Pickup Trigger
}
