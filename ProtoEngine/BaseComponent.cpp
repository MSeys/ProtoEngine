#include "ProtoEnginePCH.h"
#include "BaseComponent.h"
#include <cassert>

#include "GameObject.h"

void BaseComponent::RootInitialize()
{
	assert(!m_IsInitialized);
	if (m_IsInitialized)
		return;

	Initialize();

	m_IsInitialized = true;
}

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
