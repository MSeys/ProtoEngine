#pragma once
#include "BaseBehaviour.h"

class Transform final : public BaseBehaviour
{
public:
	Transform() : BaseBehaviour(0, true) {}
	virtual ~Transform() = default;

	Transform(const Transform& other) = delete;
	Transform(Transform&& other) noexcept = delete;
	Transform& operator=(const Transform& other) = delete;
	Transform& operator=(Transform&& other) noexcept = delete;
	
	glm::vec2 GetPosition() const;
	glm::vec2 GetRotCenter() const;
	float GetRotAngle() const;
	glm::vec2 GetScale() const;
	
	void SetPosition(float x, float y);
	void SetRotCenter(float x, float y);
	void SetRotAngle(float angle);
	void SetScale(float x, float y);
	
protected:
	void DrawInspectorTitle() override;
	void DrawInspector() override;
	
	void Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent) override;
private:
	glm::vec2 m_Position{ 0, 0 };
	glm::vec2 m_RotCenter{ 0, 0 };
	float m_RotAngle{ 0 };
	glm::vec2 m_Scale{ 1, 1 };
};
