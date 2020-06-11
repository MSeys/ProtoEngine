#include "ProtoEnginePCH.h"
#include "ProtoGui.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

void ProtoGui::Presets::InputXY(const std::vector<std::string>& texts, float& x, float& y, const DragData& data, int id)
{
	ImGui::PushID(id);

	ProtoGuiData pgData{ true, 0, -1, true, 70 };

	ImGui::Text(texts[0].c_str());
	ImGui::SameLine(100);

	pgData.sameLineOffset = 115;
	ProtoGui::Drag<float>(texts[1], pgData, "##PRESET_INPUT_X", x, data);
	ImGui::SameLine(200);

	pgData.sameLineOffset = 215;
	ProtoGui::Drag<float>(texts[2], pgData, "##PRESET_INPUT_Y", y, data);

	ImGui::PopID();
}

void ProtoGui::Presets::Color(const SDL_Color& oldColor, SDL_Color& color, int id)
{
	ImGui::PushID(id);

	float colors[4] = { float(oldColor.r) / 255.f, float(oldColor.g) / 255.f, float(oldColor.b) / 255.f, float(oldColor.a) / 255.f };
	ImGui::PushItemWidth(250);
	ImGui::ColorEdit4("##PRESET_COLOR", &colors[0], ImGuiColorEditFlags_NoInputs);
	color = { Uint8(colors[0] * 255), Uint8(colors[1] * 255), Uint8(colors[2] * 255), Uint8(colors[3] * 255) };
	ImGui::PopItemWidth();

	ImGui::PopID();
}

std::vector<std::string> ProtoGui::Presets::Path(const std::string& text, float sameLineOffset, std::string& relPath,
	const std::string& openFileTitle, const std::string& fullPath, const std::vector<std::string>& filters, int id)
{
	std::vector<std::string> selection;
	
	ImGui::PushID(id);
	
	ImGui::Text(text.c_str());
	ImGui::SameLine(sameLineOffset);

	ImGui::PushItemWidth(175);
	ImGui::InputText("##FilePathText", &relPath[0], 300, ImGuiInputTextFlags_ReadOnly);
	ImGui::PopItemWidth();

	ImGui::SameLine(sameLineOffset + 185);
	if (ImGui::Button("...##FilePathButton"))
		selection = pfd::open_file(openFileTitle, fullPath, filters).result();

	ImGui::PopID();

	return selection;
}


// Derived from https://github.com/ocornut/imgui/issues/1496
void ProtoGui::Presets::BeginGroupPanel(const char* name, const ImVec2& size)
{
	ImGui::BeginGroup();

	auto cursorPos = ImGui::GetCursorScreenPos();
	auto itemSpacing = ImGui::GetStyle().ItemSpacing;
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

	auto frameHeight = ImGui::GetFrameHeight();
	ImGui::BeginGroup();

	ImVec2 effectiveSize = size;
	if (size.x < 0.0f)
		effectiveSize.x = ImGui::GetContentRegionAvailWidth();
	else
		effectiveSize.x = size.x;
	ImGui::Dummy(ImVec2(effectiveSize.x, 0.0f));

	ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
	ImGui::SameLine(0.0f, 0.0f);
	ImGui::BeginGroup();
	ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
	ImGui::SameLine(0.0f, 0.0f);
	ImGui::TextUnformatted(name);
	ImGui::SameLine(0.0f, 0.0f);
	ImGui::Dummy(ImVec2(0.0, frameHeight + itemSpacing.y));
	ImGui::BeginGroup();

	ImGui::PopStyleVar(2);

	ImGui::GetCurrentWindow()->ContentRegionRect.Max.x -= frameHeight * 0.5f;
	ImGui::GetCurrentWindow()->Size.x -= frameHeight;

	ImGui::PushItemWidth(effectiveSize.x - frameHeight);
}

ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)
{
	return ImVec2{ lhs.x + rhs.x, lhs.y + rhs.y };
}

ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs)
{
	return ImVec2{ lhs.x - rhs.x, lhs.y - rhs.y };
}

ImVec2 operator*(const ImVec2& lhs, const float& rhs)
{
	return ImVec2{ lhs.x * rhs, lhs.y * rhs };
}

void ProtoGui::Presets::EndGroupPanel()
{
	ImGui::PopItemWidth();

	auto itemSpacing = ImGui::GetStyle().ItemSpacing;

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

	auto frameHeight = ImGui::GetFrameHeight();

	ImGui::EndGroup();

	//ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(0, 255, 0, 64), 4.0f);

	ImGui::EndGroup();

	ImGui::SameLine(0.0f, 0.0f);
	ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
	ImGui::Dummy(ImVec2(0.0, frameHeight - frameHeight * 0.5f - itemSpacing.y));

	ImGui::EndGroup();

	auto itemMin = ImGui::GetItemRectMin();
	auto itemMax = ImGui::GetItemRectMax();
	//ImGui::GetWindowDrawList()->AddRectFilled(itemMin, itemMax, IM_COL32(255, 0, 0, 64), 4.0f);

	ImVec2 halfFrame = ImVec2(frameHeight * 0.25f, frameHeight) * 0.5f;
	ImGui::GetWindowDrawList()->AddRect(
		itemMin + halfFrame, itemMax - ImVec2(halfFrame.x, 0.0f),
		ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)),
		halfFrame.x);

	ImGui::PopStyleVar(2);

	ImGui::GetCurrentWindow()->ContentRegionRect.Max.x += frameHeight * 0.5f;
	ImGui::GetCurrentWindow()->Size.x += frameHeight;

	ImGui::Dummy(ImVec2(0.0f, 0.0f));

	ImGui::EndGroup();
}

void ProtoGui::Presets::BeginComponentPanel(const ImVec2& size, BaseBehaviour* pCurrComp, BaseBehaviour** pDelComp)
{
	ImGui::BeginGroup();

	auto cursorPos = ImGui::GetCursorScreenPos();
	auto itemSpacing = ImGui::GetStyle().ItemSpacing;
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

	auto frameHeight = ImGui::GetFrameHeight();
	ImGui::BeginGroup();

	ImVec2 effectiveSize = size;
	if (size.x < 0.0f)
		effectiveSize.x = ImGui::GetContentRegionAvailWidth();
	else
		effectiveSize.x = size.x;
	ImGui::Dummy(ImVec2(effectiveSize.x, 0.0f));

	ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
	ImGui::SameLine(0.0f, 0.0f);
	ImGui::BeginGroup();
	ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
	ImGui::SameLine(0.0f, 0.0f);

	pCurrComp->DrawInspectorTitle();
	ImGui::SameLine(200);
	ImGui::Text(std::string("(ID: " + std::to_string(pCurrComp->GetID()) + ")").c_str());
	
	if (!dynamic_cast<Transform*>(pCurrComp))
	{
		ImGui::SameLine(270);

		if (ImGui::Button("^", { 20, 20 }))
			pCurrComp->GetGameObject()->SwapUpComponent(pCurrComp);

		ImGui::SameLine(300);

		if (ImGui::Button("v", { 20, 20 }))
			pCurrComp->GetGameObject()->SwapDownComponent(pCurrComp);

		ImGui::SameLine(330);

		if (ImGui::Button("X", { 20, 20 }))
			*pDelComp = pCurrComp;
	}
	
	ImGui::SameLine(0.0f, 0.0f);
	ImGui::Dummy(ImVec2(0.0, frameHeight + itemSpacing.y));
	ImGui::BeginGroup();

	ImGui::PopStyleVar(2);

	ImGui::GetCurrentWindow()->ContentRegionRect.Max.x -= frameHeight * 0.5f;
	ImGui::GetCurrentWindow()->Size.x -= frameHeight;

	ImGui::PushItemWidth(effectiveSize.x - frameHeight);
}

void ProtoGui::Presets::EndComponentPanel()
{
	EndGroupPanel();
}
