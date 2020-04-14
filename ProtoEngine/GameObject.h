#pragma once

class BaseComponent;
class TransformComponent;
class BaseScene;

class GameObject
{
public:
	GameObject(std::string name = "GameObject");
	virtual ~GameObject();

	GameObject(const GameObject& other) = delete;
	GameObject(GameObject&& other) = delete;
	GameObject& operator=(const GameObject& other) = delete;
	GameObject& operator=(GameObject&& other) = delete;

	void AddChild(GameObject* obj);
	void RemoveChild(GameObject* obj);

	void AddComponent(BaseComponent* pComp);
	void RemoveComponent(BaseComponent* pComp);

	void SwapUpChild(GameObject* obj);
	void SwapDownChild(GameObject* obj);

	void SwapUpComponent(BaseComponent* pComp);
	void SwapDownComponent(BaseComponent* pComp);
	
	TransformComponent* GetTransform() const { return m_pTransform; }
	BaseScene* GetScene() const;
	GameObject* GetParent() const { return m_pParentObject; }
	std::string& GetName() { return m_Name; }

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

	bool DrawInspector();
	
protected:
	virtual void Initialize() {}
	virtual void Draw() {}
	virtual void PostDraw() {}
	virtual void Update() {}
	virtual void FixedUpdate() {}

	void DrawHierarchy();
	
private:
	friend class BaseScene;
	
	void RootInitialize();
	void RootUpdate();
	void RootFixedUpdate();
	void RootDraw();

	std::vector<GameObject*> m_pChildren;
	std::vector<BaseComponent*> m_pComponents;

	std::string m_Name;
	bool m_IsInitialized, m_IsActive;
	BaseScene* m_pParentScene;
	GameObject* m_pParentObject;
	TransformComponent* m_pTransform;
};
