#pragma once
#include "Singleton.h"
#include <Box2D.h>

#include "GameObject.h"

struct Collision
{
	GameObject* other;
	b2Contact* contact;
};

namespace Proto
{
	class PhysicsListener : public b2ContactListener
	{
	public:
		void BeginContact(b2Contact* contact) override;
		void EndContact(b2Contact* contact) override;
		void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;
		void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;
	};
	
	class PhysicsManager final : public Singleton<PhysicsManager>
	{
	public:
		PhysicsManager();
		
		void Init();
		void Destroy();

		void Step();
		void ClearWorld();
		
		void SetPixelsPerMeter(const int ppm) { m_PixelsPerMeter = ppm; }
		void SetGravity(const b2Vec2& vec);
		void SetVelocityIterations(const int it) { m_VelocityIterations = it; }
		void SetPositionIterations(const int it) { m_PositionIterations = it; }
		
		b2Vec2 GetGravity() const { return m_Gravity; }
		b2World& GetWorld() { return m_PhysicsWorld; }
		int GetPPM() const { return m_PixelsPerMeter; }
		
	private:		
		b2Vec2 m_Gravity{ 0, 9.81f };
		b2World m_PhysicsWorld;
		PhysicsListener m_PhysicsListener;
		
		int m_VelocityIterations{ 8 };
		int m_PositionIterations{ 3 };
		int m_PixelsPerMeter{ 100 };
	};
}