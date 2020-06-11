#include "ProtoEnginePCH.h"
#include "BoxCollider2D.h"

BoxCollider2D::BoxCollider2D(ComponentID ID, bool isActive, const glm::vec2& collisionPos, const glm::vec2& collisionSize, float rotation, float density, float friction, float restitution, bool isTrigger)
	: Collider2D(ID, isActive, density, friction, restitution, isTrigger)
	, m_CollisionPos(collisionPos), m_CollisionSize(collisionSize), m_Rotation(rotation)
{
}

void BoxCollider2D::DrawInspectorTitle()
{
	DrawActiveCheckbox();
	ImGui::Text("Box Collider 2D");
}

void BoxCollider2D::DrawInspector()
{
	ProtoGui::Presets::InputXY({ "Position", "X", "Y" }, m_CollisionPos.x, m_CollisionPos.y, { 1.f, 0, 0, "%.0f" }, 0);
	ProtoGui::Presets::InputXY({ "Size", "X", "Y" }, m_CollisionSize.x, m_CollisionSize.y, { 0.1f, 0, 0, "%.1f" }, 1);

	const ProtoGui::ProtoGuiData pgData{ true, 100, -1, true, 70 };
	const ProtoGui::DragData dragData{ 1.f, 0, 0, "%.0f" };
	ProtoGui::Drag<float>("Rotation", pgData, "##BoxColliderRotation", m_Rotation, dragData);
	Collider2D::DrawInspector();
}

void BoxCollider2D::DrawEditorDebug()
{
	if(m_HasStarted)
	{
		b2PolygonShape* pCollisionShape{ dynamic_cast<b2PolygonShape*>(m_pCollision->GetShape()) };

		std::vector<glm::vec2> vertices;
		vertices.reserve(4);

		for (size_t i{}; i < 4; i++)
		{
			const b2Vec2 worldPoint{ m_pCollision->GetBody()->GetWorldPoint(pCollisionShape->m_vertices[i]) };
			vertices.push_back(ProtoConvert::Box2DToPixels(ProtoConvert::ToGLMVec(worldPoint)));
		}

		ProtoRenderer.RenderLinePolygon(vertices, { 0, 255, 0, 255 });
	}

	else
	{
		b2PolygonShape boxShape;

		const glm::vec2 collisionPos{ ProtoConvert::PixelsToBox2D(m_CollisionPos) };
		const glm::vec2 collisionSize{ ProtoConvert::PixelsToBox2D(m_CollisionSize * GetTransform()->GetScale()) };
		
		boxShape.SetAsBox(collisionSize.x, collisionSize.y,  ProtoConvert::ToBox2DVec(collisionPos), ProtoConvert::ToRadians(m_Rotation));
		
		std::vector<glm::vec2> vertices;
		vertices.reserve(4);

		for (size_t i{}; i < 4; i++)
		{
			// gameobject translation (same as the rigidbody)
			const glm::vec2 worldPoint{ GetTransform()->GetPosition() + ProtoConvert::Box2DToPixels(ProtoConvert::ToGLMVec(boxShape.m_vertices[i])) };

			// gameobject rotation  (same as the rigidbody)
			vertices.push_back(ProtoConvert::RotatePoint(worldPoint, GetTransform()->GetPosition(), ProtoConvert::ToRadians(GetTransform()->GetRotation())));
		}

		ProtoRenderer.RenderLinePolygon(vertices, { 0, 255, 0, 255 });
	}
}

void BoxCollider2D::Start()
{
	const auto pRigidBody{ Gameobject.GetComponent<RigidBody2D>() };
	if (!pRigidBody)
	{
		ProtoLogger.AddLog(LogLevel::Warning, "No RigidBody2D found on GameObject \'"
			+ Gameobject.GetName() +
			"\', BoxCollider2D (ID: " + std::to_string(GetID()) + ") was not created.");
		return;
	}

	if (!pRigidBody->IsReady())
		pRigidBody->ReadyFromCollision();

	b2PolygonShape boxShape;

	const glm::vec2 collisionPos{ ProtoConvert::PixelsToBox2D(m_CollisionPos) };
	const glm::vec2 collisionSize{ ProtoConvert::PixelsToBox2D(m_CollisionSize * GetTransform()->GetScale()) };

	boxShape.SetAsBox(collisionSize.x, collisionSize.y, ProtoConvert::ToBox2DVec(collisionPos), ProtoConvert::ToRadians(m_Rotation));
	
	b2FixtureDef def;
	def.shape = &boxShape;
	def.density = m_Density;
	def.friction = m_Friction;
	def.restitution = m_Restitution;
	def.isSensor = m_IsTrigger;
	def.userData = this;
	m_pCollision = pRigidBody->GetBody()->CreateFixture(&def);

	m_HasStarted = true;
}

void BoxCollider2D::Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent)
{
	using namespace rapidxml;

	xml_node<>* pComp = doc.allocate_node(node_element, doc.allocate_string(ProtoConvert::ToString<BoxCollider2D>().c_str()));

	SaveID(doc, pComp);
	SaveActive(doc, pComp);

	ProtoSaver::XML::Save<float>("CollisionPosX", m_CollisionPos.x, doc, pComp);
	ProtoSaver::XML::Save<float>("CollisionPosY", m_CollisionPos.y, doc, pComp);
	ProtoSaver::XML::Save<float>("CollisionSizeX", m_CollisionSize.x, doc, pComp);
	ProtoSaver::XML::Save<float>("CollisionSizeY", m_CollisionSize.y, doc, pComp);
	ProtoSaver::XML::Save<float>("CollisionRotation", m_Rotation, doc, pComp);

	Collider2D::Save(doc, pComp);

	pParent->append_node(pComp);
}

void BoxCollider2D::Load(rapidxml::xml_node<>* pComp, GameObject* pCurr)
{
	const auto id{ProtoParser::XML::Parse<unsigned int>(pComp, "ID") };
	const auto isActive{ProtoParser::XML::Parse<bool>(pComp, "Active") };

	const glm::vec2 collisionPos
	{
		ProtoParser::XML::Parse<float>(pComp, "CollisionPosX"),
		ProtoParser::XML::Parse<float>(pComp, "CollisionPosY")
	};
	
	const glm::vec2 collisionSize
	{
		ProtoParser::XML::Parse<float>(pComp, "CollisionSizeX"),
		ProtoParser::XML::Parse<float>(pComp, "CollisionSizeY")
	};

	const auto rotation{ ProtoParser::XML::Parse<float>(pComp, "CollisionRotation") };
	
	const auto density{ ProtoParser::XML::Parse<float>(pComp, "Density") };
	const auto friction{ ProtoParser::XML::Parse<float>(pComp, "Friction") };
	const auto restitution{ ProtoParser::XML::Parse<float>(pComp, "Restitution") };
	const auto isTrigger{ ProtoParser::XML::Parse<bool>(pComp, "IsTrigger") };

	const auto pBoxCollider = new BoxCollider2D(id, isActive, collisionPos, collisionSize, rotation, density, friction, restitution, isTrigger);
	pCurr->AddComponent(pBoxCollider);
}
