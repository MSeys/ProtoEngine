#pragma once

class TransformComponent;
class GameObject;

class BaseComponent
{
	friend class GameObject;
	void RootInitialize();
	
public:
	BaseComponent() = default;
	virtual ~BaseComponent() = default;
	
	BaseComponent(const BaseComponent& other) = delete;
	BaseComponent(BaseComponent&& other) noexcept = delete;
	BaseComponent& operator=(const BaseComponent& other) = delete;
	BaseComponent& operator=(BaseComponent&& other) noexcept = delete;

	GameObject* GetGameObject() const { return m_pGameObject; }
	TransformComponent* GetTransform() const;

protected:
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	GameObject* m_pGameObject{};
	bool m_IsInitialized{};
};

