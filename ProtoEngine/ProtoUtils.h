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

namespace ProtoConvert
{
	std::string ToString(const std::wstring& wstring);
	std::wstring ToWString(const std::string& string);

	template<typename T>
	std::string ToString()
	{
		const std::string compName{ typeid(T).name() };
		return compName.substr(compName.find_last_of(' ') + 1);
	}

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

	glm::vec2 PixelsToBox2D(const glm::vec2& point);
	glm::vec2 Box2DToPixels(const glm::vec2& point);
	glm::vec2 RotatePoint(const glm::vec2& point, const glm::vec2& center, float angle);
}

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
			const std::string parsed{ ParseString(pComp, name) };
			if (typeid(T) == typeid(int))
				return T(parsed.empty() ? 0 : stoi(parsed));

			if (typeid(T) == typeid(unsigned int))
				return T(parsed.empty() ? 0 : stoul(parsed));

			if (typeid(T) == typeid(Uint8))
				return T(Uint8(parsed.empty() ? 0 : stoi(parsed)));

			if (typeid(T) == typeid(float))
				return T(parsed.empty() ? 0.f : stof(parsed));

			if (typeid(T) == typeid(bool))
				return T(ParseString(pComp, name) == "true");

			return T();
		}

		template<typename T = BaseBehaviour>
		void Load(rapidxml::xml_node<>* pComponents, GameObject* pCurr, bool multiple = true)
		{
			using namespace rapidxml;
			
			if(multiple)
			{
				for (xml_node<>* pComp = pComponents->first_node(ProtoConvert::ToString<T>().c_str()); pComp; pComp = pComp->next_sibling(ProtoConvert::ToString<T>().c_str()))
				{
					const ComponentID orderID{ Parse<unsigned int>(pComp, "OrderID") };
					T::Load(pComp, pCurr);
					pCurr->GetComponents().back()->SetOrderID(orderID);
				}
			}

			else
			{
				xml_node<>* pNode{ pComponents->first_node(ProtoConvert::ToString<T>().c_str()) };
				if (pNode)
				{
					const ComponentID orderID{ Parse<unsigned int>(pNode, "OrderID") };
					T::Load(pNode, pCurr);
					pCurr->GetComponents().back()->SetOrderID(orderID);
				}
			}
		}
	}
}