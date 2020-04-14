#include "ProtoEnginePCH.h"
#include "GameObject.h"

#include <utility>

#include "BaseScene.h"
#include "BaseComponent.h"
#include "TransformComponent.h"
#include "Utils.h"

GameObject::GameObject(std::string name) :
	m_pChildren(std::vector<GameObject*>()),
	m_pComponents(std::vector<BaseComponent*>()),
	m_Name(std::move(name)),
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

	SafeDelete(obj);
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

	SafeDelete(pComp);
}
#pragma endregion Add / Remove Component

void GameObject::SwapUpChild(GameObject* obj)
{
	const auto it{ std::find(m_pChildren.begin(), m_pChildren.end(), obj) };
	if(it != m_pChildren.begin())
	{
		std::swap(*it, *(it - 1));		
	}
}

void GameObject::SwapDownChild(GameObject* obj)
{
	const auto it{ std::find(m_pChildren.begin(), m_pChildren.end(), obj) };
	if (it != m_pChildren.end() - 1)
	{
		std::swap(*it, *(it + 1));
	}
}

void GameObject::SwapUpComponent(BaseComponent* pComp)
{
	const auto it{ std::find(m_pComponents.begin(), m_pComponents.end(), pComp) };
	if (it != m_pComponents.begin() + 1)
	{
		std::swap(*it, *(it - 1));
	}
}

void GameObject::SwapDownComponent(BaseComponent* pComp)
{
	const auto it{ std::find(m_pComponents.begin(), m_pComponents.end(), pComp) };
	if (it != m_pComponents.end() - 1)
	{
		std::swap(*it, *(it + 1));
	}
}

#pragma region Root Functions
void GameObject::DrawHierarchy()
{
	std::stringstream thisAddress;
	thisAddress << this;

	std::string labelText;

	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
	if (ProtoEditor.GetCurrentSelected() == this)
		nodeFlags |= ImGuiTreeNodeFlags_Selected;

	// Leaf
	if(m_pChildren.empty())
	{
		nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		ImGui::TreeNodeEx(this, nodeFlags, m_Name.c_str());

		if (ImGui::IsItemClicked())
			ProtoEditor.SetCurrentSelected(this);

		labelText = "NodeMenu##" + thisAddress.str();
		ImGui::OpenPopupOnItemClick(labelText.c_str(), ImGuiMouseButton_Right);

		if (ImGui::BeginPopup(labelText.c_str()))
		{
			ImGui::Text("Actions");
			ImGui::Separator();

			if (ImGui::Selectable("Add GameObject"))
				AddChild(new GameObject(m_Name));

			if(ImGui::Selectable("Swap Up"))
			{
				if (m_pParentObject)
					m_pParentObject->SwapUpChild(this);

				else
					m_pParentScene->SwapUpChild(this);
			}

			if(ImGui::Selectable("Swap Down"))
			{
				if (m_pParentObject)
					m_pParentObject->SwapDownChild(this);

				else
					m_pParentScene->SwapDownChild(this);
			}

			if(ImGui::Selectable("Delete"))
			{
				if (m_pParentObject)
					m_pParentObject->RemoveChild(this);

				else
					m_pParentScene->RemoveChild(this);

				if (ProtoEditor.GetCurrentSelected() == this)
					ProtoEditor.SetCurrentSelected(nullptr);
			}

			ImGui::EndPopup();
		}
	}

	// Tree
	else
	{
		const bool nodeOpen = ImGui::TreeNodeEx(this, nodeFlags, m_Name.c_str());

		if (ImGui::IsItemClicked())
			ProtoEditor.SetCurrentSelected(this);

		labelText = "NodeMenu##" + thisAddress.str();

		ImGui::OpenPopupOnItemClick(labelText.c_str(), ImGuiMouseButton_Right);

		if (ImGui::BeginPopup(labelText.c_str()))
		{
			ImGui::Text("Actions");
			ImGui::Separator();

			if (ImGui::Selectable("Add GameObject"))
				AddChild(new GameObject(m_Name));

			if (ImGui::Selectable("Swap Up"))
			{
				if (m_pParentObject)
					m_pParentObject->SwapUpChild(this);

				else
					m_pParentScene->SwapUpChild(this);
			}

			if (ImGui::Selectable("Swap Down"))
			{
				if (m_pParentObject)
					m_pParentObject->SwapDownChild(this);

				else
					m_pParentScene->SwapDownChild(this);
			}

			if (ImGui::Selectable("Delete"))
			{
				if (m_pParentObject)
					m_pParentObject->RemoveChild(this);

				else
					m_pParentScene->RemoveChild(this);

				if (ProtoEditor.GetCurrentSelected() == this)
					ProtoEditor.SetCurrentSelected(nullptr);
			}

			ImGui::EndPopup();
		}

		if (nodeOpen)
		{
			if (!m_pChildren.empty())
			{
				for (GameObject* pChild : m_pChildren)
				{
					pChild->DrawHierarchy();
				}

				ImGui::TreePop();
			}
		}
	}
}

bool GameObject::DrawInspector()
{
	std::stringstream thisAddress;
	thisAddress << this;

	std::string labelText;
	BaseComponent* delComp{};
	
	m_Name.resize(50);

	labelText = "##Active" + thisAddress.str();
	ImGui::Checkbox(labelText.c_str(), &m_IsActive);
	ImGui::SameLine();

	labelText = "##Name" + thisAddress.str();
	ImGui::InputText(labelText.c_str(), &m_Name[0], 50);

	ImGui::SameLine(310);

	if (ImGui::Button("^"))
	{
		if (m_pParentObject)
			m_pParentObject->SwapUpChild(this);

		else
			m_pParentScene->SwapUpChild(this);
	}

	ImGui::SameLine(335);
	
	if (ImGui::Button("v"))
	{
		if (m_pParentObject)
			m_pParentObject->SwapDownChild(this);

		else
			m_pParentScene->SwapDownChild(this);
	}
	
	ImGui::SameLine(360);
	
	if(ImGui::Button("X"))
	{
		if (m_pParentObject)
			m_pParentObject->RemoveChild(this);

		else
			m_pParentScene->RemoveChild(this);

		return false;
	}
	
	for (BaseComponent* pComp : m_pComponents)
	{
		std::stringstream compAddress;
		compAddress << pComp;
		
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::SameLine();

		pComp->DrawInspectorTitle();

		if (!dynamic_cast<TransformComponent*>(pComp))
		{
			ImGui::SameLine(310);

			labelText = "^##" + compAddress.str();
			if (ImGui::Button(labelText.c_str(), { 20, 0 }))
				SwapUpComponent(pComp);
			
			ImGui::SameLine(335);
			
			labelText = "v##" + compAddress.str();
			if (ImGui::Button(labelText.c_str(), { 20, 0 }))
				SwapDownComponent(pComp);
			
			ImGui::SameLine(360);

			labelText = "X##" + compAddress.str();
			if (ImGui::Button(labelText.c_str(), { 20, 0 }))
				delComp = pComp;
		}
		
		pComp->DrawInspector();
	}

	if (delComp)
		RemoveComponent(delComp);

	return true;
}

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
	if (!m_IsActive)
		return;
	
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

void GameObject::RootFixedUpdate()
{
	if (!m_IsActive)
		return;

	FixedUpdate();

	//Component Update
	for (BaseComponent* pComp : m_pComponents)
	{
		pComp->FixedUpdate();
	}

	//Root-Object Update
	for (GameObject* pChild : m_pChildren)
	{
		pChild->RootFixedUpdate();
	}
}

void GameObject::RootDraw()
{
	if (!m_IsActive)
		return;
	
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
