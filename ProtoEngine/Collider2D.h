#pragma once
#include "BaseBehaviour.h"

class Collider2D : public BaseBehaviour
{
public:
	Collider2D(ComponentID ID, bool isActive, float density = 1, float friction = 0.3f, float restitution = 0, bool isTrigger = false);

	void DrawInspectorTitle() override = 0;
	void DrawInspector() override;
	void DrawEditorDebug() override = 0;

	b2Fixture* GetCollision() const { return m_pCollision; }
	
protected:
	b2Fixture* m_pCollision{};
	float m_Density{ 1.f };
	float m_Friction{ 0.3f };
	float m_Restitution{ 0.f };
	bool m_IsTrigger{ false };
	
	void Start() override = 0;
	void Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent) override;
};

