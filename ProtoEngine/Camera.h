#pragma once
#include "BaseBehaviour.h"

class Camera final : public BaseBehaviour
{
public:
	Camera(ComponentID ID, const glm::vec2& position, bool active);
	~Camera();
	
	void DrawInspectorTitle() override;
	void DrawInspector() override;
	
	// Moves the Camera to destination based on given speed
	void Move(const glm::vec2& destination, glm::vec2 speed);
	
	// Moves the Camera to destination based on given time
	void Move(const glm::vec2& destination, float time);

	//Moves the Camera to destination immediately
	void Move(const glm::vec2& destination);

	// Stops the movement of camera and resets all variables
	void Reset();
	
	bool IsMoving() const { return m_IsMovingBySpeed || m_IsMovingByTime; }
	bool IsActive() const { return m_IsCamActive; }
	
	void Activate();
	void Deactivate();
	
	glm::vec2 GetPosition() const { return m_Position; }
	
protected:
	void Start() override;
	void Update() override;
	void Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent) override;

public:
	void DrawEditorDebug() override;
	
private:
	glm::vec2 m_Position{}, m_Destination{};
	glm::vec2 m_Speed{ -1, -1 };
	float m_CurrTime{}, m_Time{};
	float m_LerpAlphaX{}, m_LerpAlphaY{};
	bool m_IsMovingBySpeed{}, m_IsMovingByTime{};
	bool m_IsCamActive{};

public:
	static void Load(rapidxml::xml_node<>* pComp, GameObject* pCurr);
};