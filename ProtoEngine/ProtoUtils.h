
#pragma once
#include "GameObject.h"
#include "Components.h"

#pragma warning(push)
#pragma warning (disable:4201)
#include <glm/glm.hpp>
#pragma warning(pop)

#include <string>
#include <codecvt>
#include <locale>

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

float SecondsToMicroSeconds(float seconds);
float MicroSecondsToSeconds(float microSeconds);

std::string WStringToString(const std::wstring& wstring);
std::wstring StringToWString(const std::string& string);

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
				pComp->append_attribute(doc.allocate_attribute(name, doc.allocate_string(ToCString(value))));
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
			void LoadImageComponents(rapidxml::xml_node<>* pComponents, GameObject* pCurr);
			void LoadTextComponents(rapidxml::xml_node<>* pComponents, GameObject* pCurr);
			void LoadFPSComponents(rapidxml::xml_node<>* pComponents, GameObject* pCurr);

			void LoadTexData(rapidxml::xml_node<>* pComp, TextureData& texData);
			void LoadAlignments(rapidxml::xml_node<>* pComp, HAlignment& horAlignment, VAlignment& verAlignment);
		}
	}
}