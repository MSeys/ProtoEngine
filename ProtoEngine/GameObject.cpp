#include "ProtoEnginePCH.h"
#include "GameObject.h"

#include <utility>

#include "Scene.h"
#include "BaseBehaviour.h"
#include "Transform.h"

GameObject::GameObject(GameObjectID ID, std::string name, bool isActive)
	: m_pChildren(std::vector<GameObject*>())
	, m_pComponents(std::vector<BaseBehaviour*>())
	, m_Name(std::move(name))
	, m_ID(ID)
	, m_IsInitialized(false)
	, m_IsActive(isActive)
	, m_pParentScene(nullptr)
	, m_pParentObject(nullptr)
	, m_pTransform(nullptr)
{
	m_pTransform = new Transform();
	AddComponent(m_pTransform);
}

GameObject::~GameObject()
{
	for (BaseBehaviour* pComp : m_pComponents)
		SafeDelete(pComp);
	
	for (GameObject* pChild : m_pChildren)
		SafeDelete(pChild);
}

#pragma region Add / Remove Child
void GameObject::Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent)
{
	using namespace rapidxml;
	
	xml_node<>* thisGO = doc.allocate_node(node_element, "GameObject");
	
	ProtoSaver::XML::SaveString("Name", m_Name, doc, thisGO);
	ProtoSaver::XML::Save<GameObjectID>("ID", m_ID, doc, thisGO);
	ProtoSaver::XML::Save<bool>("Active", m_IsActive, doc, thisGO);

	xml_node<>* thisComponents = doc.allocate_node(node_element, "Components");
	
	for (size_t i{}; i < m_pComponents.size(); i++)
	{
		m_pComponents[i]->SetOrderID(unsigned(i));
		m_pComponents[i]->Save(doc, thisComponents);
	}
	
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
		const std::string goName{ ProtoParser::XML::ParseString(gameObjectNode, "Name") };
		const GameObjectID id{ ProtoParser::XML::Parse<unsigned int>(gameObjectNode, "ID") };
		const bool goActive{ ProtoParser::XML::Parse<bool>(gameObjectNode, "Active") };

		auto pNew = new GameObject(id, goName, goActive);
		AddChild(pNew);

		pNew->Load(gameObjectNode);
	}

	SortComponentsByOrder();
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
			std::cout << "GameObject::AddChild > Failed to initialize the added GameObject! (Parent GameObject is not part of a Scene)" << std::endl;

		else
			obj->Start();
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
void GameObject::AddComponent(BaseBehaviour* pComp)
{
#if _DEBUG
	if (typeid(*pComp) == typeid(Transform) && HasComponent<Transform>())
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

	if (GetCurrentID() < pComp->GetID())
		SetCurrentID(pComp->GetID());
	m_pComponents.push_back(pComp);
	pComp->m_pGameObject = this;

	if (m_IsInitialized)
	{
		pComp->Start();
		pComp->Awake();
	}
}

void GameObject::RemoveComponent(BaseBehaviour* pComp, bool deleteComp)
{
	auto it = find(m_pComponents.begin(), m_pComponents.end(), pComp);

#if _DEBUG
	if (it == m_pComponents.end())
	{
		std::cout << "GameObject::RemoveComponent > Component is not attached to this GameObject!" << std::endl;
		return;
	}

	if (typeid(*pComp) == typeid(Transform))
	{
		std::cout << "GameObject::RemoveComponent > TransformComponent can't be removed!" << std::endl;
		return;
	}
#endif

	m_pComponents.erase(it);

	if (deleteComp)
		SafeDelete(pComp);
	else
		pComp->m_pGameObject = nullptr;
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

void GameObject::SwapUpComponent(BaseBehaviour* pComp)
{
	const auto it{ std::find(m_pComponents.begin(), m_pComponents.end(), pComp) };
	if (it != m_pComponents.begin() + 1)
	{
		std::swap(*it, *(it - 1));
	}
}

void GameObject::SwapDownComponent(BaseBehaviour* pComp)
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
	GameObject* pFoundHere = *std::find_if(m_pChildren.cbegin(), m_pChildren.cend(), [id](GameObject* pObject) { return pObject->GetID() == id; });
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

BaseBehaviour* GameObject::FindComponentWithID(ComponentID c_id) const
{
	BaseBehaviour* pFoundComp = *std::find_if(m_pComponents.cbegin(), m_pComponents.cend(), [c_id](BaseBehaviour* pComp) { return pComp->GetID() == c_id; });
	if (pFoundComp)
		return pFoundComp;

	return nullptr;
}

#pragma region Root Functions
void GameObject::DrawHierarchy()
{
	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
	if (ProtoEditor.GetCurrentSelected() == this)
		nodeFlags |= ImGuiTreeNodeFlags_Selected;

	if (m_pChildren.empty())
		nodeFlags |= ImGuiTreeNodeFlags_Leaf;

	const bool open = ImGui::TreeNodeEx(this, nodeFlags, m_Name.c_str());

	ImGui::OpenPopupOnItemClick("##NodeMenu", ImGuiMouseButton_Right);

	if (ImGui::BeginPopup("##NodeMenu"))
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

void GameObject::DrawInspector()
{
	std::string labelText;
	BaseBehaviour* pDelComp{};
	
	m_Name.resize(50);

	ImGui::PushID(this);
	
	/* Inspector */ {

		/* First Row */ {
			ImGui::Checkbox("##Active", &m_IsActive);
			ImGui::SameLine();

			ImGui::InputText("##Name", &m_Name[0], 50);

			ImGui::SameLine();
			ImGui::Text(("ID: " + std::to_string(m_ID)).c_str());
		}

		for (BaseBehaviour* pComp : m_pComponents)
		{
			ImGui::PushID(pComp);
			ProtoGui::Presets::BeginComponentPanel(ImVec2{ -1, 0 }, pComp, &pDelComp);

			ImGui::Spacing();
			ImGui::Spacing();
			
			pComp->DrawInspector();

			ImGui::Spacing();
			ImGui::Spacing();
			
			ProtoGui::Presets::EndComponentPanel();
			ImGui::PopID();
		}
	}
		
	if (pDelComp)
		RemoveComponent(pDelComp, true);

	ImGui::PopID();
}

void GameObject::DrawEditorDebug()
{
	// Components DrawEditorDebug
	for (BaseBehaviour* pComp : m_pComponents)
		pComp->DrawEditorDebug();

	// Children DrawEditorDebug
	for (GameObject* pChild : m_pChildren)
		pChild->DrawEditorDebug();
}

void GameObject::Start()
{
	if (m_IsInitialized)
		return;

	if (GetScene()->GetCurrentID() < m_ID)
		GetScene()->SetCurrentID(m_ID);

	// Components Start
	for (BaseBehaviour* pComp : m_pComponents)
		pComp->Start();

	// Children Start
	for (GameObject* pChild : m_pChildren)
		pChild->Start();

	m_IsInitialized = true;
}

void GameObject::Awake()
{
	// Components Awake
	for (BaseBehaviour* pComp : m_pComponents)
		pComp->Awake();

	// Children Awake
	for (GameObject* pChild : m_pChildren)
		pChild->Awake();
}

void GameObject::Update()
{
	if (!m_IsActive)
		return;

	// Components Update
	for (BaseBehaviour* pComp : m_pComponents)
	{
		if(pComp->GetActive())
			pComp->Update();
	}

	// Children Update
	for (GameObject* pChild : m_pChildren)
		pChild->Update();
}

void GameObject::FixedUpdate()
{
	if (!m_IsActive)
		return;

	//Component Update
	for (BaseBehaviour* pComp : m_pComponents)
	{
		if(pComp->GetActive())
			pComp->FixedUpdate();
	}

	// Children FixedUpdate
	for (GameObject* pChild : m_pChildren)
		pChild->FixedUpdate();
}

void GameObject::Draw()
{
	if (!m_IsActive)
		return;

	//Components Draw
	for (BaseBehaviour* pComp : m_pComponents)
	{
		if(pComp->GetActive())
			pComp->Draw();
	}

	// Children Draw
	for (GameObject* pChild : m_pChildren)
		pChild->Draw();
}

void GameObject::OnTriggerEnter(const Collision& collision)
{
	for (BaseBehaviour* pComp : m_pComponents)
		pComp->OnTriggerEnter(collision);
}

void GameObject::OnTriggerExit(const Collision& collision)
{
	for (BaseBehaviour* pComp : m_pComponents)
		pComp->OnTriggerExit(collision);
}

void GameObject::OnCollisionEnter(const Collision& collision)
{
	for (BaseBehaviour* pComp : m_pComponents)
		pComp->OnCollisionEnter(collision);
}

void GameObject::OnCollisionExit(const Collision& collision)
{
	for (BaseBehaviour* pComp : m_pComponents)
		pComp->OnCollisionExit(collision);
}
#pragma endregion Root Functions

void GameObject::SortComponentsByOrder()
{
	std::sort(m_pComponents.begin(), m_pComponents.end(), [](BaseBehaviour* left, BaseBehaviour* right)
	{
		return left->GetOrderID() < right->GetOrderID();
	});
}

Scene* GameObject::GetScene() const
{
	if (!m_pParentScene && m_pParentObject)
		return m_pParentObject->GetScene();

	return m_pParentScene;
}
