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
	
	glm::vec3 GetPosition() const
	{
		if(GetGameObject()->GetParent())
		{
			return GetGameObject()->GetParent()->GetTransform()->GetPosition() + m_Position;
		}
		
		return m_Position;
	}
	void SetPosition(float x, float y, float z);
	
protected:
	void Initialize() override;
	void Update() override;
	void FixedUpdate() override;
	void Draw() override;

	void DrawInspectorTitle() override;
	void DrawInspector() override;
private:
	glm::vec3 m_Position;
};
