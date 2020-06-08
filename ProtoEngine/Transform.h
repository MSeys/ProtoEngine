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
	float GetRotation() const;
	glm::vec2 GetScale() const;
	
	void SetPosition(float x, float y);
	void SetRotation(float angle);
	void SetScale(float x, float y);
	
protected:
	void DrawInspectorTitle() override;
	void DrawInspector() override;
	
	void Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent) override;
	
private:
	glm::vec2 m_Position{ 0, 0 };
	float m_Rotation{ 0 };
	glm::vec2 m_Scale{ 1, 1 };

public:
	static void Load(rapidxml::xml_node<>* pComp, GameObject* pCurr);
};
