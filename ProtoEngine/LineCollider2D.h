#pragma once
#include "Collider2D.h"

class LineCollider2D : public Collider2D
{
public:
	LineCollider2D(ComponentID ID, bool isActive, const glm::vec2& posA = { -20, 0 }, const glm::vec2& posB ={ 20, 0 }, float density = 1, float friction = 0.3f, float restitution = 0, bool isTrigger = false);

	void DrawInspectorTitle() override;
	void DrawInspector() override;
	void DrawEditorDebug() override;

protected:
	void Start() override;
	void Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent) override;

private:
	glm::vec2 m_PosA{ -20, 0 };
	glm::vec2 m_PosB{ 20, 0 };
};
