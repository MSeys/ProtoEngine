#pragma once
#include "GameObject.h"
#include "EngineBehaviours.h"

#pragma warning(push)
#pragma warning (disable:4201)
#include <glm/glm.hpp>
#pragma warning(pop)

#include <string>
#include <codecvt>
#include <locale>

#include "Box2D.h"
#include "ImGui/imgui.h"

#define ToCString(value) std::to_string(value).c_str()

template<class T>
void SafeDelete(T& pObjectToDelete)
{
	if (pObjectToDelete != nullptr)
	{
		delete(pObjectToDelete);
		pObjectToDelete = nullptr;
	}
}

template<typename T>
T& VoidCast(void* pData)
{
	return *static_cast<T*>(pData);
}

template <typename T>
T Lerp(const T& x, const T& y, const float& s)
{
	return x + (y - x) * s;
}

float SecondsToMicroSeconds(float seconds);
float MicroSecondsToSeconds(float microSeconds);



namespace ProtoSaver
{
	namespace XML
	{
		void SaveString(const char* name, std::string& value, rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pComp);
		
		template<typename T>
		void Save(const char* name, T& value, rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pComp)
		{
			if(typeid(T) == typeid(bool))
				pComp->append_attribute(doc.allocate_attribute(name, doc.allocate_string(bool(value) ? "true" : "false")));

			else
			{
				
				std::string valueStr{ std::to_string(value) };
				if (valueStr.empty())
					valueStr = "\0";
				
				pComp->append_attribute(doc.allocate_attribute(name, doc.allocate_string(valueStr.c_str())));
			}
		}
	}
}

namespace ProtoParser
{
	namespace XML
	{
		void LoadComponents(rapidxml::xml_node<>* pComponents, GameObject* pCurr);

		std::string ParseString(rapidxml::xml_node<>* pComp, const std::string& name);

		template<typename T>
		T Parse(rapidxml::xml_node<>* pComp, const std::string& name)
		{
			if (typeid(T) == typeid(int))
				return T(stoi(ParseString(pComp, name)));

			if (typeid(T) == typeid(unsigned int))
				return T(stoul(ParseString(pComp, name)));

			if (typeid(T) == typeid(Uint8))
				return T(Uint8(stoi(ParseString(pComp, name))));

			if (typeid(T) == typeid(float))
				return T(stof(ParseString(pComp, name)));

			if (typeid(T) == typeid(bool))
				return T(ParseString(pComp, name) == "true");

			return T();
		}
		
		namespace Helper
		{
			void LoadTransformComponent(rapidxml::xml_node<>* pComponents, GameObject* pCurr);
			void LoadRigidBody2DComponent(rapidxml::xml_node<>* pComponents, GameObject* pCurr);
			void LoadBoxCollider2DComponents(rapidxml::xml_node<>* pComponents, GameObject* pCurr);
			void LoadSphereCollider2DComponents(rapidxml::xml_node<>* pComponents, GameObject* pCurr);
			void LoadLineCollider2DComponents(rapidxml::xml_node<>* pComponents, GameObject* pCurr);

			void LoadImageComponents(rapidxml::xml_node<>* pComponents, GameObject* pCurr);
			void LoadTextComponents(rapidxml::xml_node<>* pComponents, GameObject* pCurr);
			void LoadFPSComponents(rapidxml::xml_node<>* pComponents, GameObject* pCurr);
			void LoadCameraComponent(rapidxml::xml_node<>* pComponents, GameObject* pCurr);
			
			void LoadTexData(rapidxml::xml_node<>* pComp, TextureData& texData);
			void LoadAlignments(rapidxml::xml_node<>* pComp, HAlignment& horAlignment, VAlignment& verAlignment);
		}
	}
}

namespace ProtoConvert
{
	std::string ToString(const std::wstring& wstring);
	std::wstring ToWString(const std::string& string);

	b2Vec2 ToBox2DVec(const glm::vec2& vec);
	b2Vec2 ToBox2DVec(const ImVec2& vec);
	b2Vec3 ToBox2DVec(const glm::vec3& vec);
	
	ImVec2 ToImGuiVec(const glm::vec2& vec);
	ImVec2 ToImGuiVec(const b2Vec2& vec);
	ImVec4 ToImGuiVec(const glm::vec4& vec);

	glm::vec2 ToGLMVec(const b2Vec2& vec);
	glm::vec2 ToGLMVec(const ImVec2& vec);
	glm::vec3 ToGLMVec(const b2Vec3& vec);
	glm::vec4 ToGLMVec(const ImVec4& vec);

	float ToDegrees(float radians);
	float ToRadians(float degrees);

	Uint32 ColorToUint(int R, int G, int B);
	SDL_Color UintToColor(Uint32 color);
}