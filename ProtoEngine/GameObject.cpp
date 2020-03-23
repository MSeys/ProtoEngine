#include "ProtoEnginePCH.h"
#include "GameObject.h"

#include "BaseScene.h"
#include "BaseComponent.h"
#include "TransformComponent.h"
#include "Utils.h"

GameObject::GameObject() :
	m_pChildren(std::vector<GameObject*>()),
	m_pComponents(std::vector<BaseComponent*>()),
	m_IsInitialized(false),
	m_IsActive(true),
	m_pParentScene(nullptr),
	m_pParentObject(nullptr),
	m_pTransform(nullptr)
{
	m_pTransform = new TransformComponent();
	AddComponent(m_pTransform);
}

GameObject::~GameObject()
{
	for (BaseComponent* pComp : m_pComponents)
	{
		SafeDelete(pComp);
	}

	for (GameObject* pChild : m_pChildren)
	{
		SafeDelete(pChild);
	}
}

#pragma region Add / Remove Child
void GameObject::AddChild(GameObject* obj)
{
#if _DEBUG
	if (obj->m_pParentObject)
	{
		if (obj->m_pParentObject == this)
			std::cout << "GameObject::AddChild > GameObject to add is already attached to this parent" << std::endl;
		else
			std::cout << "GameObject::AddChild > GameObject to add is already attached to another GameObject. Detach it from it's current parent before attaching it to another one." << std::endl;

		return;
	}

	if (obj->m_pParentScene)
	{
		std::cout << "GameObject::AddChild > GameObject is currently attached to a GameScene. Detach it from it's current parent before attaching it to another one." << std::endl;
		return;
	}
#endif

	obj->m_pParentObject = this;
	m_pChildren.push_back(obj);

	if (m_IsInitialized)
	{
		const auto scene = GetScene();

		if (scene == nullptr)
		{
			std::cout << "GameObject::AddChild > Failed to initialize the added GameObject! (Parent GameObject is not part of a Scene)" << std::endl;
		}
		else
		{
			obj->RootInitialize();
		}
	}
}

void GameObject::RemoveChild(GameObject* obj)
{
	auto it = find(m_pChildren.begin(), m_pChildren.end(), obj);

#if _DEBUG
	if (it == m_pChildren.end())
	{
		std::cout << "GameObject::RemoveChild > GameObject to remove is not attached to this GameObject!" << std::endl;
		return;
	}
#endif

	m_pChildren.erase(it);
	obj->m_pParentObject = nullptr;
}
#pragma endregion Add / Remove Child

#pragma region Add / Remove Component
void GameObject::AddComponent(BaseComponent* pComp)
{
#if _DEBUG
	if (typeid(*pComp) == typeid(TransformComponent) && HasComponent<TransformComponent>())
	{
		std::cout << "GameObject::AddComponent > GameObject can contain only one TransformComponent!" << std::endl;
		return;
	}

	for (auto* component : m_pComponents)
	{
		if (component == pComp)
		{
			std::cout << "GameObject::AddComponent > GameObject already contains this component!" << std::endl;
			return;
		}
	}
#endif

	m_pComponents.push_back(pComp);
	pComp->m_pGameObject = this;
}

void GameObject::RemoveComponent(BaseComponent* pComp)
{
	auto it = find(m_pComponents.begin(), m_pComponents.end(), pComp);

#if _DEBUG
	if (it == m_pComponents.end())
	{
		std::cout << "GameObject::RemoveComponent > Component is not attached to this GameObject!" << std::endl;
		return;
	}

	if (typeid(*pComp) == typeid(TransformComponent))
	{
		std::cout << "GameObject::RemoveComponent > TransformComponent can't be removed!" << std::endl;
		return;
	}
#endif

	m_pComponents.erase(it);
	pComp->m_pGameObject = nullptr;
}
#pragma endregion Add / Remove Component

#pragma region Root Functions
void GameObject::RootInitialize()
{
	if (m_IsInitialized)
		return;

	Initialize(); // User-Object

	//Root-Component Initialization
	for (BaseComponent* pComp : m_pComponents)
	{
		pComp->RootInitialize();
	}

	//Root-Object Initialization
	for (GameObject* pChild : m_pChildren)
	{
		pChild->RootInitialize();
	}

	m_IsInitialized = true;
}

void GameObject::RootUpdate()
{
	Update();

	//Component Update
	for (BaseComponent* pComp : m_pComponents)
	{
		pComp->Update();
	}

	//Root-Object Update
	for (GameObject* pChild : m_pChildren)
	{
		pChild->RootUpdate();
	}
}

void GameObject::RootDraw()
{
	//User-Object Draw
	Draw();

	//Component Draw
	for (BaseComponent* pComp : m_pComponents)
	{
		pComp->Draw();
	}

	//Root-Object Draw
	for (GameObject* pChild : m_pChildren)
	{
		pChild->RootDraw();
	}
}
#pragma endregion Root Functions

BaseScene* GameObject::GetScene() const
{
	if (!m_pParentScene && m_pParentObject)
	{
		return m_pParentObject->GetScene();
	}

	return m_pParentScene;
}
