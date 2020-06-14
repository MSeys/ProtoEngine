#include "ProtoEnginePCH.h"
#include "BB_ZenChanController.h"

void BB_ZenChanController::DrawInspectorTitle()
{
	ImGui::Text("BB - Zen-Chan Controller");
}

void BB_ZenChanController::DrawInspector()
{
	const ProtoGui::ProtoGuiData pgData{ true, 100, -1, true, 70 };
	const ProtoGui::DragData dragData{ 0.1f, 0, 0, "%.1f" };

	ProtoGui::Drag<float>("Walk Speed", pgData, "##BB_ZenChanController_WalkSpeed", m_WalkSpeed, dragData);
	ProtoGui::Drag<float>("Jump Force", pgData, "##BB_ZenChanController_JumpForce", m_JumpForce, dragData);
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
}

void BB_ZenChanController::FixedUpdate()
{
	if (!m_pGameMode)
		return;

	float newXVelocity{};
	if (m_pGameMode->HasPlayerTwo())
	{
		Transform* pPlayerOneT{ m_pGameMode->GetPlayerOne()->GetTransform() }, *pPlayerTwoT{ m_pGameMode->GetPlayerTwo()->GetTransform() };
		const float distancePlayerOne{ distance(pPlayerOneT->GetPosition(), GetTransform()->GetPosition()) };
		const float distancePlayerTwo{ distance(pPlayerTwoT->GetPosition(), GetTransform()->GetPosition()) };

		if(distancePlayerOne < distancePlayerTwo)
		{
			if (pPlayerOneT->GetPosition().x < GetTransform()->GetPosition().x)
				newXVelocity = -1;
			else if (abs(pPlayerOneT->GetPosition().x - GetTransform()->GetPosition().x) < 2.f)
				newXVelocity = 0;
			else
				newXVelocity = 1;
		}

		else
		{
			if (pPlayerTwoT->GetPosition().x < GetTransform()->GetPosition().x)
				newXVelocity = -1;
			else if (abs(pPlayerTwoT->GetPosition().x - GetTransform()->GetPosition().x) < 2.f)
				newXVelocity = 0;
			else
				newXVelocity = 1;
		}
	}
	
	else
	{
		Transform* pPlayerOneT{ m_pGameMode->GetPlayerOne()->GetTransform() };
		
		if (pPlayerOneT->GetPosition().x < GetTransform()->GetPosition().x)
			newXVelocity = -1;
		else if (abs(pPlayerOneT->GetPosition().x - GetTransform()->GetPosition().x) < 2.f)
			newXVelocity = 0;
		else
			newXVelocity = 1;
	}

	if(newXVelocity == -1 && m_Velocity.x != -1)
		m_pAnimatedSprite->PlayAnimation(0);
	else if(newXVelocity == 1 && m_Velocity.x != 1)
		m_pAnimatedSprite->PlayAnimation(1);

	m_Velocity.x = newXVelocity;
	
	m_pRigidBody->GetBody()->SetLinearVelocity({ m_Velocity.x * m_WalkSpeed, m_pRigidBody->GetBody()->GetLinearVelocity().y });
}

void BB_ZenChanController::Draw()
{
}

void BB_ZenChanController::OnCollisionEnter(const Collision& collision)
{
	if (collision.other->GetName() == "Wall")
		return;

	m_FootContacts++;
}

void BB_ZenChanController::OnCollisionExit(const Collision& collision)
{
	if (collision.other->GetName() == "Wall")
		return;

	m_FootContacts--;
}

void BB_ZenChanController::PreSolveCollision(const Collision& collision)
{
	// Derived from https://www.iforce2d.net/b2dtut/one-way-walls

	b2Fixture* fixtureA{ collision.contact->GetFixtureA() }, * fixtureB{ collision.contact->GetFixtureB() };

	b2Fixture* enemyFixture{ static_cast<GameObject*>(fixtureA->GetBody()->GetUserData()) == GetGameObject() ? fixtureA : fixtureB };
	b2Fixture* otherFixture{ enemyFixture == fixtureA ? fixtureB : fixtureA };

	if (static_cast<GameObject*>(otherFixture->GetBody()->GetUserData())->GetName() == "Wall")
	{
		collision.contact->SetEnabled(true);
		return;
	}

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

void BB_ZenChanController::Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent)
{
	using namespace rapidxml;

	xml_node<>* pComp = doc.allocate_node(node_element, doc.allocate_string(ProtoConvert::ToString<BB_ZenChanController>().c_str()));
	SaveID(doc, pComp);
	SaveActive(doc, pComp);

	ProtoSaver::XML::Save<float>("WalkSpeed", m_WalkSpeed, doc, pComp);
	ProtoSaver::XML::Save<float>("JumpForce", m_JumpForce, doc, pComp);
	
	pParent->append_node(pComp);
}

void BB_ZenChanController::Load(rapidxml::xml_node<>* pComp, GameObject* pCurr)
{
	const auto id{ ProtoParser::XML::Parse<unsigned int>(pComp, "ID") };
	const auto isActive{ ProtoParser::XML::Parse<bool>(pComp, "Active") };

	const auto walkSpeed{ ProtoParser::XML::Parse<float>(pComp, "WalkSpeed") };
	const auto jumpForce{ ProtoParser::XML::Parse<float>(pComp, "JumpForce") };
	
	pCurr->AddComponent(new BB_ZenChanController(id, isActive, walkSpeed, jumpForce));
}