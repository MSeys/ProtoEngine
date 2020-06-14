#include "ProtoEnginePCH.h"
#include "PhysicsManager.h"

#include "Collider2D.h"

void Proto::PhysicsListener::BeginContact(b2Contact* contact)
{
	const auto fixtureA = contact->GetFixtureA(), fixtureB = contact->GetFixtureB();
	auto gameObjectA = static_cast<GameObject*>(fixtureA->GetBody()->GetUserData()), gameObjectB = static_cast<GameObject*>(fixtureB->GetBody()->GetUserData());

	if (fixtureA->IsSensor() || fixtureB->IsSensor())
	{
		gameObjectA->OnTriggerEnter({ gameObjectB, contact });
		gameObjectB->OnTriggerEnter({ gameObjectA, contact });
	}
	
	else
	{
		gameObjectA->OnCollisionEnter({ gameObjectB, contact });
		gameObjectB->OnCollisionEnter({ gameObjectA, contact });
	}
}

void Proto::PhysicsListener::EndContact(b2Contact* contact)
{
	const auto fixtureA = contact->GetFixtureA(), fixtureB = contact->GetFixtureB();

	auto gameObjectA = static_cast<GameObject*>(fixtureA->GetBody()->GetUserData()), gameObjectB = static_cast<GameObject*>(fixtureB->GetBody()->GetUserData());

	if (fixtureA->IsSensor() || fixtureB->IsSensor())
	{
		gameObjectA->OnTriggerExit({ gameObjectB, contact });
		gameObjectB->OnTriggerExit({ gameObjectA, contact });
	}
	
	else
	{
		gameObjectA->OnCollisionExit({ gameObjectB, contact });
		gameObjectB->OnCollisionExit({ gameObjectA, contact });
	}
}

void Proto::PhysicsListener::PreSolve(b2Contact* contact, const b2Manifold*)
{
	const auto fixtureA = contact->GetFixtureA(), fixtureB = contact->GetFixtureB();
	const auto colliderA = static_cast<Collider2D*>(fixtureA->GetUserData()), colliderB = static_cast<Collider2D*>(fixtureB->GetUserData());
	
	if (!colliderA->GetActive() || !colliderB->GetActive())
	{
		contact->SetEnabled(false);
		return;
	}

	auto gameObjectA = static_cast<GameObject*>(fixtureA->GetBody()->GetUserData()), gameObjectB = static_cast<GameObject*>(fixtureB->GetBody()->GetUserData());
	if (fixtureA->IsSensor() || fixtureB->IsSensor())
	{
		gameObjectA->PreSolveTrigger({ gameObjectB, contact });
		gameObjectB->PreSolveTrigger({ gameObjectA, contact });
	}

	
	else
	{
		gameObjectA->PreSolveCollision({ gameObjectB, contact });
		gameObjectB->PreSolveCollision({ gameObjectA, contact });
	}
}

void Proto::PhysicsListener::PostSolve(b2Contact*, const b2ContactImpulse*)
{
}

Proto::PhysicsManager::PhysicsManager()
	: m_PhysicsWorld(m_Gravity)
{
	m_PhysicsWorld.SetContactListener(&m_PhysicsListener);
}

void Proto::PhysicsManager::Init()
{
}

void Proto::PhysicsManager::Destroy()
{
	ClearWorld();
}

void Proto::PhysicsManager::Step()
{
	m_PhysicsWorld.Step(ProtoTime.FixedDeltaTime, m_VelocityIterations, m_PositionIterations);
}

void Proto::PhysicsManager::ClearWorld()
{
	b2Body* pBody{ m_PhysicsWorld.GetBodyList() };
	while(pBody != nullptr)
	{
		b2Body* pNext = pBody->GetNext();
		m_PhysicsWorld.DestroyBody(pBody);
		pBody = pNext;
	}
}

void Proto::PhysicsManager::SetGravity(const b2Vec2& vec)
{
	m_Gravity = vec;
	m_Gravity.y *= -1; // Invert Y due to coordinate system
	m_PhysicsWorld.SetGravity(m_Gravity);
}