#include "ProtoEnginePCH.h"
#include "BaseComponent.h"

#include "GameObject.h"

TransformComponent* BaseComponent::GetTransform() const
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
