#include "ProtoEnginePCH.h"
#include "BaseBehaviour.h"

#include "GameObject.h"

BaseBehaviour::BaseBehaviour(ComponentID ID, bool isActive)
	: m_ID(ID), m_IsActive(isActive)
{
}

Transform* BaseBehaviour::GetTransform() const
{
#if _DEBUG
	if(!m_pGameObject)
	{
		std::cout << "BaseComponent::GetTransform() > Failed to retrieve the TransformComponent. GameObject is NULL." << std::endl;
		return nullptr;
	}
#endif

	return m_pGameObject->GetTransform();
}

void BaseBehaviour::SaveID(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent)
{
	ProtoSaver::XML::Save<unsigned int>("ID", m_ID, doc, pParent);
}

void BaseBehaviour::SaveActive(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent)
{
	ProtoSaver::XML::Save<bool>("Active", m_IsActive, doc, pParent);
}

void BaseBehaviour::DrawActiveCheckbox()
{
	ImGui::Checkbox("", &m_IsActive);
	ImGui::SameLine(0, 5);
}
