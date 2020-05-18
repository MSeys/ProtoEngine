#pragma once
#include "Collider2D.h"

class BoxCollider2D : public Collider2D
{
public:
	BoxCollider2D(ComponentID ID, bool isActive, const glm::vec2& collisionSize = { 50, 50 }, float density = 1, float friction = 0.3f, float restitution = 0, bool isTrigger = false);
	
	void DrawInspectorTitle() override;
	void DrawInspector() override;
	void DrawEditorDebug() override;

protected:
	void Start() override;
	void Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent) override;

private:
	glm::vec2 m_CollisionSize{ 50, 50 };
};
