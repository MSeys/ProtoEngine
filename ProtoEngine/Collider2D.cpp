#include "ProtoEnginePCH.h"
#include "Collider2D.h"

Collider2D::Collider2D(ComponentID ID, bool isActive, float density, float friction, float restitution, bool isTrigger)
	: BaseBehaviour(ID, isActive)
	, m_Density(density), m_Friction(friction), m_Restitution(restitution), m_IsTrigger(isTrigger)
{
}

void Collider2D::DrawInspector()
{
	const ProtoGui::ProtoGuiData pgData{ true, 100, -1, true, 70 };
	const ProtoGui::DragData dragData{ 0.1f, 0, 0, "%.1f" };

	ProtoGui::Drag<float>("Density", pgData, "##BOX_COL_DENSITY", m_Density, dragData);
	ProtoGui::Drag<float>("Friction", pgData, "##BOX_COL_FRICTION", m_Friction, dragData);
	ProtoGui::Drag<float>("Restitution", pgData, "##BOX_COL_RESTITUTION", m_Restitution, dragData);

	ImGui::Text("Is Trigger:");
	ImGui::SameLine();
	ImGui::Checkbox("##BOX_COL_TRIGGER", &m_IsTrigger);
}

void Collider2D::Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent)
{
	ProtoSaver::XML::Save<float>("Density", m_Density, doc, pParent);
	ProtoSaver::XML::Save<float>("Friction", m_Friction, doc, pParent);
	ProtoSaver::XML::Save<float>("Restitution", m_Restitution, doc, pParent);

	ProtoSaver::XML::Save<bool>("IsTrigger", m_IsTrigger, doc, pParent);
}
