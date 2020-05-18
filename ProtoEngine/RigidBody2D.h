#pragma once
#include "BaseBehaviour.h"

enum class RigidBodyType
{
	STATIC, DYNAMIC, KINEMATIC
};

class RigidBody2D final : public BaseBehaviour
{
public:
	RigidBody2D(ComponentID ID, RigidBodyType bodyType);
	
	void DrawInspectorTitle() override;
	void DrawInspector() override;

	b2Body* GetBody() const { return m_pBody; }

	bool IsReady() const { return m_IsReady; }
	void ReadyFromCollision() { Start(); }
	
protected:
	void Start() override;
	void FixedUpdate() override;
	void Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent) override;
	
private:
	b2Body* m_pBody{};
	RigidBodyType m_BodyType{ RigidBodyType::STATIC };
	const bool m_FixedRotation{ true };
	bool m_IsReady{ false };
};