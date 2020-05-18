#pragma once
#include "Structs.h"
#include <string>

#define ImColor_Blue ImVec4{ 0, 118.f / 255.f, 210.f / 255.f, 1.f }  // NOLINT(cppcoreguidelines-macro-usage)
namespace ProtoGui
{
	struct ProtoGuiData
	{
		bool useSameLine = false;
		float sameLineOffset = 0;
		float sameLineSpacing = -1;
		bool useItemWidth = false;
		float itemWidth = 0;
	};
	
	struct DragData
	{
		float v_speed = 1.f;
		float v_min = 0;
		float v_max = 0;
		std::string format;
		float power = 1.f;
	};
	
	template<typename T>
	void Drag(const std::string& text, const ProtoGuiData& pgData, const std::string& label, T& value, const DragData& data)
	{
		if (!text.empty())
		{
			ImGui::Text(text.c_str());
			if (pgData.useSameLine)
				ImGui::SameLine(pgData.sameLineOffset, pgData.sameLineSpacing);
		}

		if (pgData.useItemWidth)
			ImGui::PushItemWidth(pgData.itemWidth);
		
		if(typeid(T) == typeid(int))
			ImGui::DragInt(label.c_str(), reinterpret_cast<int*>(&value), data.v_speed, int(data.v_min), int(data.v_max), data.format.c_str());

		else if(typeid(T) == typeid(float))
			ImGui::DragFloat(label.c_str(), reinterpret_cast<float*>(&value), data.v_speed, float(data.v_min), float(data.v_max), data.format.c_str(), data.power);

		else
			ImGui::Text("Type not compatible.");

		if (pgData.useItemWidth)
			ImGui::PopItemWidth();
	}
	
	namespace Presets
	{
		void Position(float& x, float& y, int id = 0);
		void Size(float& w, float& h, int id = 0);
		void Alignment(HAlignment& hAlignment, VAlignment& vAlignment, int id = 0);
		void Color(const std::string& text, const TextureData& texData, SDL_Color& color, int id = 0);

		// Derived from https://github.com/ocornut/imgui/issues/1496
		void BeginGroupPanel(const char* name, const ImVec2& size = ImVec2(-1.0f, -1.0f));
		void EndGroupPanel();

		// Derived from https://github.com/ocornut/imgui/issues/1496
		void BeginComponentPanel(const ImVec2& size, BaseBehaviour* pCurrComp, BaseBehaviour** pDelComp);
		void EndComponentPanel();
	}
}