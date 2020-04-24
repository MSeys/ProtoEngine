
#pragma once
#include "GameObject.h"
#include "Components.h"

#pragma warning(push)
#pragma warning (disable:4201)
#include <glm/glm.hpp>
#pragma warning(pop)

#include <string>

namespace ProtoParser
{
	namespace XML
	{
		void LoadComponents(rapidxml::xml_node<>* pComponents, GameObject* pCurr);

		int ParseInt(rapidxml::xml_node<>* pComp, const std::string& name);
		unsigned int ParseUInt(rapidxml::xml_node<>* pComp, const std::string& name);
		float ParseFloat(rapidxml::xml_node<>* pComp, const std::string& name);
		Uint8 ParseUint8(rapidxml::xml_node<>* pComp, const std::string& name);
		std::string ParseString(rapidxml::xml_node<>* pComp, const std::string& name);
		
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