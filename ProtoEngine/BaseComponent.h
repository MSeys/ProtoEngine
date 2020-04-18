#pragma once

class TransformComponent;
class GameObject;

class BaseComponent
{
	friend class GameObject;
	
public:
	BaseComponent() = default;
	virtual ~BaseComponent() = default;
	
	BaseComponent(const BaseComponent& other) = delete;
	BaseComponent(BaseComponent&& other) noexcept = delete;
	BaseComponent& operator=(const BaseComponent& other) = delete;
	BaseComponent& operator=(BaseComponent&& other) noexcept = delete;

	GameObject* GetGameObject() const { return m_pGameObject; }
	TransformComponent* GetTransform() const;

	virtual void DrawInspectorTitle() {}
	virtual void DrawInspector() = 0;
	
protected:
	virtual void Start() {}
	virtual void Awake() {}
	virtual void Update() {}
	virtual void FixedUpdate() {}
	virtual void Draw() {}

	virtual void Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent) = 0;

	GameObject* m_pGameObject{};
};

