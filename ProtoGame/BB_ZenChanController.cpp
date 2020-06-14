#include "ProtoEnginePCH.h"
#include "BB_ZenChanController.h"


#include "BB_PickupController.h"
#include "BB_PlayerController.h"

void BB_ZenChanController::DrawInspectorTitle()
{
	ImGui::Text("BB - Zen-Chan Controller");
}

void BB_ZenChanController::DrawInspector()
{
	const ProtoGui::ProtoGuiData pgData{ true, 120, -1, true, 70 };
	const ProtoGui::DragData dragData{ 0.1f, 0, 0, "%.1f" };

	ProtoGui::Drag<float>("Walk Speed", pgData, "##BB_ZenChanController_WalkSpeed", m_WalkSpeed, dragData);
	ProtoGui::Drag<float>("Jump Force", pgData, "##BB_ZenChanController_JumpForce", m_JumpForce, dragData);
	ProtoGui::Drag<float>("Jump Cooldown", pgData, "##BB_ZenChanController_JumpCooldown", m_JumpCooldown, dragData);
	ProtoGui::Drag<float>("Stuck Cooldown", pgData, "##BB_ZenChanController_StuckInBubbleCooldown", m_StuckInBubbleCooldown, dragData);
}

void BB_ZenChanController::Start()
{
}

void BB_ZenChanController::Awake()
{
	GameObject* pGameModeObject{ Gameobject.GetScene()->FindGameObjectInRootWithName("GameMode") };
	if(pGameModeObject)
		m_pGameMode = pGameModeObject->GetComponent<BB_GameMode>();

	m_pRigidBody = Gameobject.GetComponent<RigidBody2D>();
	m_pAnimatedSprite = Gameobject.GetComponent<AnimatedSprite>();
	m_pBoxCollider = Gameobject.GetComponent<BoxCollider2D>();
}

void BB_ZenChanController::Update()
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

	if(m_StuckInBubble)
	{
		m_StuckInBubbleCooldownTimer += ProtoTime.DeltaTime;
		if(m_StuckInBubbleCooldownTimer > m_StuckInBubbleCooldown)
		{
			m_StuckInBubble = false;
			m_pRigidBody->GetBody()->SetGravityScale(1.f);
			m_StuckInBubbleCooldownTimer = 0.f;

			if(m_WasHit)
			{
				SpawnPickup();
				Gameobject.Destroy();
			}
		}
	}
}

void BB_ZenChanController::FixedUpdate()
{
	Move();

	if (!m_pGameMode->IsTransitioning())
	{
		if (GetTransform()->GetPosition().y > ProtoSettings.GetWindowSize().y + 50.f)
			m_pRigidBody->GetBody()->SetTransform(ProtoConvert::ToBox2DVec(ProtoConvert::PixelsToBox2D({ GetTransform()->GetPosition().x, -10.f })), 0);
	}
}

void BB_ZenChanController::Draw()
{
}

void BB_ZenChanController::OnCollisionEnter(const Collision& collision)
{
	if (collision.other->GetName() == "Walls")
		return;

	if(collision.other->GetName() == "Bubble")
	{
		if (!m_StuckInBubble)
			m_pAnimatedSprite->PlayAnimation(2);
		
		m_StuckInBubble = true;
		m_pRigidBody->GetBody()->SetGravityScale(-0.02f);
		m_pRigidBody->GetBody()->SetLinearVelocity({ 0, 0 });
		m_Velocity = { 0, 0 };
		
		return;
	}

	if(collision.other == m_pGameMode->GetPlayerOne()->GetGameObject())
	{
		if (m_StuckInBubble)
		{
			m_pAnimatedSprite->PlayAnimation(3);
			m_WasHit = true;
			m_pGameMode->GetPlayerOne()->AddScore(25);
		}
		return;
	}

	if(m_pGameMode->HasPlayerTwo())
	{
		if (collision.other == m_pGameMode->GetPlayerTwo()->GetGameObject())
		{
			if (m_StuckInBubble)
			{
				m_pAnimatedSprite->PlayAnimation(3);
				m_WasHit = true;
				m_pGameMode->GetPlayerTwo()->AddScore(25);
			}
			return;
		}
	}
	
	m_FootContacts++;
}

void BB_ZenChanController::OnCollisionExit(const Collision& collision)
{
	if (collision.other->GetName() == "Walls")
		return;

	m_FootContacts--;
}

void BB_ZenChanController::PreSolveCollision(const Collision& collision)
{
	b2Fixture* fixtureA{ collision.contact->GetFixtureA() }, * fixtureB{ collision.contact->GetFixtureB() };

	b2Fixture* enemyFixture{ static_cast<GameObject*>(fixtureA->GetBody()->GetUserData()) == GetGameObject() ? fixtureA : fixtureB };

	if(m_StuckInBubble)
	{
		collision.contact->SetEnabled(true);
		return;
	}

	
	if(collision.other->GetName() == "Floors")
	{
		// Derived from https://www.iforce2d.net/b2dtut/one-way-walls
		b2Body* playerBody = enemyFixture->GetBody();

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

void BB_ZenChanController::Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent)
{
	using namespace rapidxml;

	xml_node<>* pComp = doc.allocate_node(node_element, doc.allocate_string(ProtoConvert::ToString<BB_ZenChanController>().c_str()));
	SaveID(doc, pComp);
	SaveActive(doc, pComp);

	ProtoSaver::XML::Save<float>("WalkSpeed", m_WalkSpeed, doc, pComp);
	ProtoSaver::XML::Save<float>("JumpForce", m_JumpForce, doc, pComp);
	ProtoSaver::XML::Save<float>("JumpCooldown", m_JumpCooldown, doc, pComp);
	ProtoSaver::XML::Save<float>("StuckInBubbleCooldown", m_StuckInBubbleCooldown, doc, pComp);
	
	pParent->append_node(pComp);
}

void BB_ZenChanController::Move()
{
	if (!m_pGameMode)
		return;

	if (m_StuckInBubble)
		return;

	Transform* pTarget;
	float newXVelocity;

#pragma region Assign Target
	if (m_pGameMode->HasPlayerTwo())
	{
		Transform* pPlayerOneT{ m_pGameMode->GetPlayerOne()->GetTransform() }, * pPlayerTwoT{ m_pGameMode->GetPlayerTwo()->GetTransform() };
		const float distancePlayerOne{ distance(pPlayerOneT->GetPosition(), GetTransform()->GetPosition()) };
		const float distancePlayerTwo{ distance(pPlayerTwoT->GetPosition(), GetTransform()->GetPosition()) };

		pTarget = { distancePlayerOne < distancePlayerTwo ? pPlayerOneT : pPlayerTwoT };
	}

	else
	{
		pTarget = m_pGameMode->GetPlayerOne()->GetTransform();
	}

#pragma endregion Assign Target
	
	if (pTarget->GetPosition().x < GetTransform()->GetPosition().x)
		newXVelocity = -1;
	else if (abs(pTarget->GetPosition().x - GetTransform()->GetPosition().x) < 2.f)
		newXVelocity = 0;
	else
		newXVelocity = 1;

	if (newXVelocity == -1 && m_Velocity.x != -1)
		m_pAnimatedSprite->PlayAnimation(0);
	else if (newXVelocity == 1 && m_Velocity.x != 1)
		m_pAnimatedSprite->PlayAnimation(1);

	m_Velocity.x = newXVelocity;

	const float velChange = m_Velocity.x * m_WalkSpeed - m_pRigidBody->GetBody()->GetLinearVelocity().x;
	const float impulse = m_pRigidBody->GetBody()->GetMass() * velChange;
	m_pRigidBody->GetBody()->ApplyLinearImpulse({ impulse, 0 }, m_pRigidBody->GetBody()->GetWorldCenter(), true);

	if (pTarget->GetPosition().y < GetTransform()->GetPosition().y && abs(pTarget->GetPosition().y - GetTransform()->GetPosition().y) > 5.f)
		Jump();
}

void BB_ZenChanController::Jump()
{	
	if (m_FootContacts > 0 && m_CanJump)
	{
		m_pRigidBody->GetBody()->ApplyLinearImpulse({ 0, m_pRigidBody->GetBody()->GetMass() * m_JumpForce }, m_pRigidBody->GetBody()->GetWorldCenter(), true);
		m_CanJump = false;
	}
}

void BB_ZenChanController::SpawnPickup() const
{
	GameObject* pPickup{ new GameObject(Gameobject.GetScene()->RequestNewID(), "Pickup", true) };
	Gameobject.GetParent()->AddChild(pPickup, true);
	pPickup->GetTransform()->SetPosition(GetTransform()->GetPosition().x, GetTransform()->GetPosition().y);

	pPickup->AddComponent(new Image(pPickup->RequestNewID(), true, ProtoContent.GetTexture("\\BubbleBobble\\Bubble_Bobble_item_watermelon.png"), {}, { 0.5f, 0.5f }, { 0.5f, 1.f }), true);
	pPickup->AddComponent(new RigidBody2D(pPickup->RequestNewID(), RigidBodyType::DYNAMIC, 1.f, true, false), true);
	pPickup->AddComponent(new BoxCollider2D(pPickup->RequestNewID(), true, { 0, -20 }, { 15, 13 }, 0, 1.f, 0.f, 0.f, true), true);
	pPickup->AddComponent(new BoxCollider2D(pPickup->RequestNewID(), true, { 0, -20 }, { 15, 13 }, 0, 1.f, 0.f, 0.f, false), true);
	pPickup->AddComponent(new BB_PickupController(pPickup->RequestNewID(), true, 100), true);
}

void BB_ZenChanController::Load(rapidxml::xml_node<>* pComp, GameObject* pCurr)
{
	const auto id{ ProtoParser::XML::Parse<unsigned int>(pComp, "ID") };
	const auto isActive{ ProtoParser::XML::Parse<bool>(pComp, "Active") };

	const auto walkSpeed{ ProtoParser::XML::Parse<float>(pComp, "WalkSpeed") };
	const auto jumpForce{ ProtoParser::XML::Parse<float>(pComp, "JumpForce") };
	const auto jumpCooldown{ ProtoParser::XML::Parse<float>(pComp, "JumpCooldown") };
	const auto stuckInBubbleCooldown{ ProtoParser::XML::Parse<float>(pComp, "StuckInBubbleCooldown") };
	
	pCurr->AddComponent(new BB_ZenChanController(id, isActive, walkSpeed, jumpForce, jumpCooldown, stuckInBubbleCooldown));
}
