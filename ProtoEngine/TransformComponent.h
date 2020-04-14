#pragma once
#include "BaseComponent.h"

#pragma warning(push)
#pragma warning (disable:4201)
#include <glm/vec3.hpp>

#pragma warning(pop)

class TransformComponent : public BaseComponent
{
public:
	TransformComponent() = default;
	virtual ~TransformComponent() = default;

	TransformComponent(const TransformComponent& other) = delete;
	TransformComponent(TransformComponent&& other) noexcept = delete;
	TransformComponent& operator=(const TransformComponent& other) = delete;
	TransformComponent& operator=(TransformComponent&& other) noexcept = delete;
	
	glm::vec2 GetPosition() const
	{
		if(GetGameObject()->GetParent())
		{
			return GetGameObject()->GetParent()->GetTransform()->GetPosition() + m_Position;
		}
		
		return m_Position;
	}

	glm::vec2 GetScale() const
	{
		if (GetGameObject()->GetParent())
		{
			return GetGameObject()->GetParent()->GetTransform()->GetScale() + m_Scale;
		}

		return m_Scale;
	}
	
	void SetPosition(float x, float y);
	void SetScale(float x, float y);
	
protected:
	void Initialize() override;
	void Update() override;
	void FixedUpdate() override;
	void Draw() override;

	void DrawInspectorTitle() override;
	void DrawInspector() override;
private:
	glm::vec2 m_Position;
	glm::vec2 m_Scale;
};
