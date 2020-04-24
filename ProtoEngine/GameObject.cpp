#include "ProtoEnginePCH.h"
#include "GameObject.h"

#include <utility>

#include "Scene.h"
#include "BaseComponent.h"
#include "ProtoParser.h"
#include "TransformComponent.h"
#include "Utils.h"

GameObject::GameObject(GameObjectID ID, std::string name, bool isActive)
	: m_pChildren(std::vector<GameObject*>())
	, m_pComponents(std::vector<BaseComponent*>())
	, m_Name(std::move(name))
	, m_ID(ID)
	, m_IsInitialized(false)
	, m_IsActive(isActive)
	, m_pParentScene(nullptr)
	, m_pParentObject(nullptr)
	, m_pTransform(nullptr)
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
void GameObject::Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent)
{
	using namespace rapidxml;
	
	xml_node<>* thisGO = doc.allocate_node(node_element, "GameObject");
	thisGO->append_attribute(doc.allocate_attribute("Name", doc.allocate_string(m_Name.c_str())));
	thisGO->append_attribute(doc.allocate_attribute("ID", doc.allocate_string(ToCString(m_ID))));
	thisGO->append_attribute(doc.allocate_attribute("Active", m_IsActive ? "true" : "false"));

	xml_node<>* thisComponents = doc.allocate_node(node_element, "Components");
	
	for(BaseComponent* pComp : m_pComponents)
		pComp->Save(doc, thisComponents);
	
	thisGO->append_node(thisComponents);

	for (GameObject* pChild : m_pChildren)
		pChild->Save(doc, thisGO);
	
	pParent->append_node(thisGO);
}

void GameObject::Load(rapidxml::xml_node<>* pNode)
{
	using namespace rapidxml;

	xml_node<>* pComponentsNode = pNode->first_node("Components");
	ProtoParser::XML::LoadComponents(pComponentsNode, this);

	for (xml_node<>* gameObjectNode = pNode->first_node("GameObject"); gameObjectNode; gameObjectNode = gameObjectNode->next_sibling())
	{
		const std::string goName{ gameObjectNode->first_attribute("Name")->value() };
		const GameObjectID id{ ProtoParser::XML::ParseUInt(gameObjectNode, "ID") };
		const bool goActive{ std::string(gameObjectNode->first_attribute("Active")->value()) == "true" };

		auto pNew = new GameObject(id, goName, goActive);
		AddChild(pNew);

		pNew->Load(gameObjectNode);
	}
}

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
			obj->Start();
		}
	}
}

void GameObject::RemoveChild(GameObject* obj, bool deleteObject)
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

	if(deleteObject)
		SafeDelete(obj);
	else
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

void GameObject::MakeChild()
{
	if(m_pParentObject)
	{
		// Already a child of another gameobject >> level deeper, child above this
		const auto it{ std::find(m_pParentObject->GetChildren().begin(), m_pParentObject->GetChildren().end(), this) };
		if (it != m_pParentObject->GetChildren().begin())
		{
			GameObject* newParent = *(it - 1);
			m_pParentObject->RemoveChild(this, false);
			newParent->AddChild(this);
		}
	}

	else
	{
		// GameObject is in root of scene >> level deeper, gameobject above this
		const auto it{ std::find(m_pParentScene->GetChildren().begin(), m_pParentScene->GetChildren().end(), this) };
		if (it != m_pParentScene->GetChildren().begin())
		{
			GameObject* newParent = *(it - 1);
			m_pParentScene->RemoveChild(this, false);
			newParent->AddChild(this);
		}
	}
}

void GameObject::MakeParent()
{
	if (!m_pParentObject) // Gameobject is in root
		return;

	if(m_pParentObject->GetParent()) // Does this parent have a parent?
	{
		auto newParent = m_pParentObject->GetParent();
		m_pParentObject->RemoveChild(this, false);
		newParent->AddChild(this);
	}

	else // This parent is part of root (scene)
	{
		auto scene = m_pParentObject->GetScene();
		m_pParentObject->RemoveChild(this, false);
		scene->AddChild(this);
	}
}

GameObject* GameObject::FindGameObjectWithIDinChildren(GameObjectID id)
{
	GameObject* pFoundHere = *std::find(m_pChildren.cbegin(), m_pChildren.cend(), [id](GameObject* pObject) { return pObject->GetID() == id; });
	if (pFoundHere)
		return pFoundHere;
	
	for (GameObject* pGameObject : m_pChildren)
	{
		GameObject* pFoundInChildren = pGameObject->FindGameObjectWithIDinChildren(id);
		if (pFoundInChildren)
			return pFoundInChildren;
	}

	return nullptr;
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

	if (m_pChildren.empty())
		nodeFlags |= ImGuiTreeNodeFlags_Leaf;

	const bool open = ImGui::TreeNodeEx(this, nodeFlags, m_Name.c_str());

	labelText = "##NodeMenu" + thisAddress.str();
	ImGui::OpenPopupOnItemClick(labelText.c_str(), ImGuiMouseButton_Right);

	if (ImGui::BeginPopup(labelText.c_str()))
	{
		ImGui::Text("Actions");
		ImGui::Separator();

		if (ImGui::Selectable("Add GameObject"))
			AddChild(new GameObject(GetScene()->RequestNewID(), m_Name));

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

		if (ImGui::Selectable("Make Child"))
		{
			MakeChild();
		}

		if (ImGui::Selectable("Make Parent"))
		{
			MakeParent();
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
	
	if (ImGui::IsItemClicked())
		ProtoEditor.SetCurrentSelected(this);

	if (open)
	{
		for (GameObject* pChild : m_pChildren)
		{
			pChild->DrawHierarchy();
		}

		ImGui::TreePop();
	}
}

bool GameObject::DrawInspector()
{
	std::stringstream thisAddress;
	thisAddress << this;

	std::string labelText;
	BaseComponent* pDelComp{};
	
	m_Name.resize(50);

	/* Inspector */ {

		/* First Row */ {
			labelText = "##Active" + thisAddress.str();
			ImGui::Checkbox(labelText.c_str(), &m_IsActive);
			ImGui::SameLine();

			labelText = "##Name" + thisAddress.str();
			ImGui::InputText(labelText.c_str(), &m_Name[0], 50);

			ImGui::SameLine();
			ImGui::Text(("ID: " + std::to_string(m_ID)).c_str());
		}

		for (BaseComponent* pComp : m_pComponents)
		{
			std::stringstream compAddress;
			compAddress << pComp;

			ImGuiProto::BeginComponentPanel(ImVec2{ -1, 0 }, pComp, &pDelComp, compAddress.str());

			ImGui::Spacing();
			ImGui::Spacing();
			
			pComp->DrawInspector();

			ImGui::Spacing();
			ImGui::Spacing();
			
			ImGuiProto::EndComponentPanel();
		}
	}
		
	if (pDelComp)
		RemoveComponent(pDelComp);

	return true;
}

void GameObject::Start()
{
	if (m_IsInitialized)
		return;

	if (GetScene()->GetCurrentID() < m_ID)
		GetScene()->SetCurrentID(m_ID);

	// Components Start
	for (BaseComponent* pComp : m_pComponents)
	{
		pComp->Start();
	}

	// Children Start
	for (GameObject* pChild : m_pChildren)
	{
		pChild->Start();
	}

	m_IsInitialized = true;
}

void GameObject::Awake()
{
	// Components Awake
	for (BaseComponent* pComp : m_pComponents)
	{
		pComp->Awake();
	}

	// Children Awake
	for (GameObject* pChild : m_pChildren)
	{
		pChild->Awake();
	}
}

void GameObject::Update()
{
	if (!m_IsActive)
		return;

	// Components Update
	for (BaseComponent* pComp : m_pComponents)
	{
		pComp->Update();
	}

	// Children Update
	for (GameObject* pChild : m_pChildren)
	{
		pChild->Update();
	}
}

void GameObject::FixedUpdate()
{
	if (!m_IsActive)
		return;

	//Component Update
	for (BaseComponent* pComp : m_pComponents)
	{
		pComp->FixedUpdate();
	}

	// Children FixedUpdate
	for (GameObject* pChild : m_pChildren)
	{
		pChild->FixedUpdate();
	}
}

void GameObject::Draw()
{
	if (!m_IsActive)
		return;

	//Components Draw
	for (BaseComponent* pComp : m_pComponents)
	{
		pComp->Draw();
	}

	// Children Draw
	for (GameObject* pChild : m_pChildren)
	{
		pChild->Draw();
	}
}
#pragma endregion Root Functions

Scene* GameObject::GetScene() const
{
	if (!m_pParentScene && m_pParentObject)
	{
		return m_pParentObject->GetScene();
	}

	return m_pParentScene;
}
