#pragma once
#include "RapidXML/rapidxml.hpp"

class BaseComponent;
class TransformComponent;
class Scene;

using GameObjectID = unsigned int;

class GameObject
{
public:
	GameObject(GameObjectID ID, std::string name = "GameObject", bool isActive = true);
	virtual ~GameObject();
	
	GameObject(const GameObject& other) = delete;
	GameObject(GameObject&& other) = delete;
	GameObject& operator=(const GameObject& other) = delete;
	GameObject& operator=(GameObject&& other) = delete;

	void Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent);
	void Load(rapidxml::xml_node<>* pNode);
	
	void AddChild(GameObject* obj);
	void RemoveChild(GameObject* obj, bool deleteObject = true);

	void AddComponent(BaseComponent* pComp);
	void RemoveComponent(BaseComponent* pComp);

	void SwapUpChild(GameObject* obj);
	void SwapDownChild(GameObject* obj);

	void SwapUpComponent(BaseComponent* pComp);
	void SwapDownComponent(BaseComponent* pComp);

	void MakeChild();
	void MakeParent();

	GameObject* FindGameObjectWithIDinChildren(GameObjectID id);
	
	TransformComponent* GetTransform() const { return m_pTransform; }
	Scene* GetScene() const;
	GameObject* GetParent() const { return m_pParentObject; }
	std::string& GetName() { return m_Name; }
	GameObjectID GetID() const { return m_ID; }

#pragma region
	template <class T>
	bool HasComponent(bool searchChildren = false)
	{
		return GetComponent<T>(searchChildren) != nullptr;
	}

	template <class T>
	T* GetComponent(bool searchChildren = false)
	{
		const type_info& ti = typeid(T);
		for (auto* component : m_pComponents)
		{
			if (component && typeid(*component) == ti)
				return static_cast<T*>(component);
		}

		if (searchChildren)
		{
			for (auto* child : m_pChildren)
			{
				if (child->GetComponent<T>(searchChildren) != nullptr)
					return child->GetComponent<T>(searchChildren);
			}
		}

		return nullptr;
	}

	template <class T>
	std::vector<T*> GetComponents(bool searchChildren = false)
	{
		const type_info& ti = typeid(T);
		std::vector<T*> components;

		for (auto* component : m_pComponents)
		{
			if (component && typeid(*component) == ti)
				components.push_back(static_cast<T*>(component));
		}

		if (searchChildren)
		{
			for (auto* child : m_pChildren)
			{
				auto childComponents = child->GetComponents<T>(searchChildren);

				for (auto* childComp : childComponents)
					components.push_back(static_cast<T*>(childComp));
			}
		}

		return components;
	}

	template <class T>
	T* GetChild()
	{
		const type_info& ti = typeid(T);
		for (auto* child : m_pChildren)
		{
			if (child && typeid(*child) == ti)
				return static_cast<T*>(child);
		}
		return nullptr;
	}

	template <class T>
	std::vector<T*> GetChildren()
	{
		const type_info& ti = typeid(T);
		std::vector<T*> children;

		for (auto* child : m_pChildren)
		{
			if (child && typeid(*child) == ti)
				children.push_back((T*)child);
		}
		return children;
	}
#pragma endregion Template Methods

	std::vector<GameObject*>& GetChildren() { return m_pChildren; }
	
	bool DrawInspector();
	
protected:
	void DrawHierarchy();
	
private:
	friend class Scene;
	
	void Start();
	void Awake();
	void Update();
	void FixedUpdate();
	void Draw();

	std::vector<GameObject*> m_pChildren;
	std::vector<BaseComponent*> m_pComponents;

	std::string m_Name;
	GameObjectID m_ID;
	bool m_IsInitialized, m_IsActive;
	Scene* m_pParentScene;
	GameObject* m_pParentObject;
	TransformComponent* m_pTransform;
};
