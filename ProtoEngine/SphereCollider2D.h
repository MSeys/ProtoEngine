#pragma once
#include "Collider2D.h"

class SphereCollider2D : public Collider2D
{
public:
	SphereCollider2D(ComponentID ID, bool isActive, glm::vec2 position = { 0, 0 }, float radius = 25.f, float density = 1, float friction = 0.3f, float restitution = 0, bool isTrigger = false);

	void DrawInspectorTitle() override;
	void DrawInspector() override;
	void DrawEditorDebug() override;

protected:
	void Start() override;
	void Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent) override;

private:
	glm::vec2 m_Position{ 0, 0 };
	float m_Radius{ 25.f };
	bool m_HasStarted{ false };

public:
	static void Load(rapidxml::xml_node<>* pComp, GameObject* pCurr);
};