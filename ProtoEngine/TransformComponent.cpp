#include "ProtoEnginePCH.h"
#include "TransformComponent.h"

void TransformComponent::SetPosition(const float x, const float y, const float z)
{
	m_Position.x = x;
	m_Position.y = y;
	m_Position.z = z;
}

void TransformComponent::Initialize()
{
	m_Position = { 0, 0, 0 };
}

void TransformComponent::Update()
{
}

void TransformComponent::Draw()
{
}
