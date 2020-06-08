#pragma once
#include "Scene.h"

class Transform;
class GameObject;

using GameObjectID = unsigned int;
using ComponentID = unsigned int;

#define Gameobject (*GetGameObject())

struct FindData
{
	GameObjectID GO_ID;
	ComponentID C_ID;
};

class BaseBehaviour
{
	friend class GameObject;
	
public:
	BaseBehaviour(ComponentID ID, bool isActive = true);
	virtual ~BaseBehaviour() = default;
	
	BaseBehaviour(const BaseBehaviour& other) = delete;
	BaseBehaviour(BaseBehaviour&& other) noexcept = delete;
	BaseBehaviour& operator=(const BaseBehaviour& other) = delete;
	BaseBehaviour& operator=(BaseBehaviour&& other) noexcept = delete;

	GameObject* GetGameObject() const { return m_pGameObject; }
	Transform* GetTransform() const;

	GameObjectID GetID() const { return m_ID; }
	bool GetActive() const
	{
		if(m_pGameObject)
			return m_pGameObject->GetActive() && m_IsActive;

		return m_IsActive;
	}
	void SetActive(bool active) { m_IsActive = active; }

	virtual void DrawInspectorTitle() {}
	virtual void DrawInspector() = 0;
	virtual void DrawEditorDebug() {}

	ComponentID GetOrderID() const { return m_OrderID; }
	void SetOrderID(const ComponentID id) { m_OrderID = id; }
	
protected:
	virtual void Start() {}
	virtual void Awake() {}
	virtual void Update() {}
	virtual void FixedUpdate() {}
	virtual void Draw() {}

	virtual void OnTriggerEnter(const Collision& collision) { UNREFERENCED_PARAMETER(collision); }
	virtual void OnTriggerExit(const Collision& collision) { UNREFERENCED_PARAMETER(collision); }
	virtual void OnCollisionEnter(const Collision& collision) { UNREFERENCED_PARAMETER(collision); }
	virtual void OnCollisionExit(const Collision& collision) { UNREFERENCED_PARAMETER(collision); }
	
	virtual void Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent) = 0;
	
	void SaveID(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent);
	void SaveActive(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent);
	void DrawActiveCheckbox();
	
	template<typename T>
	T* Find(const FindData& findData, const bool inChildren = false)
	{
		if (!GetGameObject())
			return nullptr;

		GameObject* foundGameObject;
		if (inChildren)
			foundGameObject = GetGameObject()->FindGameObjectWithIDinChildren(findData.GO_ID);
		else
			foundGameObject = GetGameObject()->GetScene()->FindGameObjectWithID(findData.GO_ID);

		if (!foundGameObject)
			return nullptr;
		
		const auto foundBehaviour = foundGameObject->FindComponentWithID(findData.C_ID);
		if (!foundBehaviour)
			return nullptr;
		
		const auto castFoundBehaviour = dynamic_cast<T*>(foundBehaviour);
		if (!castFoundBehaviour)
			return nullptr;

		return castFoundBehaviour;
	}
	
	GameObject* m_pGameObject{};

private:
	ComponentID m_ID, m_OrderID;
	bool m_IsActive{ true };
};

