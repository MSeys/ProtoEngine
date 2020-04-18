#include "ProtoEnginePCH.h"
#include "ImGui_Proto.h"

#include "ContentManager.h"
#include "Font.h"
#include "Texture2D.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

void ImGuiProto::Position(TextureData& texData, const std::string& address)
{
	std::string labelText;

	/* Position */ {
		ImGui::Text("Position");

		/* Position X */ {
			ImGui::SameLine(100);
			ImGui::Text("X");
			ImGui::SameLine(115);

			ImGui::PushItemWidth(70);
			labelText = "##POS_X" + address;
			ImGui::DragFloat(labelText.c_str(), &texData.x, 1, 0, 0, "%.1f");
			ImGui::PopItemWidth();
		}

		/* Position Y */ {
			ImGui::SameLine(200);
			ImGui::Text("Y");
			ImGui::SameLine(215);

			ImGui::PushItemWidth(70);
			labelText = "##POS_Y" + address;
			ImGui::DragFloat(labelText.c_str(), &texData.y, 1, 0, 0, "%.1f");
			ImGui::PopItemWidth();
		}
	}
}

void ImGuiProto::Size(TextureData& texData, const std::string& address)
{
	std::string labelText;
	
	/* Size */ {
		ImGui::Text("Size");

		ImGui::SameLine(100);
		ImGui::Text("W");
		ImGui::SameLine(115);

		ImGui::PushItemWidth(70);
		labelText = "##SIZE_WIDTH" + address;
		ImGui::DragFloat(labelText.c_str(), &texData.width, 1, 0, 0, "%.1f");
		ImGui::PopItemWidth();

		ImGui::SameLine(200);
		ImGui::Text("H");
		ImGui::SameLine(215);

		ImGui::PushItemWidth(70);
		labelText = "##SIZE_HEIGHT" + address;
		ImGui::DragFloat(labelText.c_str(), &texData.height, 1, 0, 0, "%.1f");
		ImGui::PopItemWidth();
	}
}

void ImGuiProto::Alignment(HAlignment& hAlignment, VAlignment& vAlignment)
{
	/* Alignment */ {
		ImGui::Text("Alignment");

		ImGui::SameLine(115);

		const ImVec4 selectedCol{ 0.4f, 0.4f, 0.4f, 1.f };

		/* Horizontal Alignment */ {
			if (hAlignment == HAlignment::LEFT)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, selectedCol);
				ImGui::ImageButton(ProtoContent.GetTexture("Engine/align-left.png")->GetSDLTexture(), { 18, 18 });
				ImGui::PopStyleColor();
			}

			else
			{
				if (ImGui::ImageButton(ProtoContent.GetTexture("Engine/align-left.png")->GetSDLTexture(), { 18, 18 }))
					hAlignment = HAlignment::LEFT;
			}

			ImGui::SameLine();

			if (hAlignment == HAlignment::CENTER)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, selectedCol);
				ImGui::ImageButton(ProtoContent.GetTexture("Engine/align-center.png")->GetSDLTexture(), { 18, 18 });
				ImGui::PopStyleColor();
			}

			else
			{
				if (ImGui::ImageButton(ProtoContent.GetTexture("Engine/align-center.png")->GetSDLTexture(), { 18, 18 }))
					hAlignment = HAlignment::CENTER;
			}

			ImGui::SameLine();

			if (hAlignment == HAlignment::RIGHT)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, selectedCol);
				ImGui::ImageButton(ProtoContent.GetTexture("Engine/align-right.png")->GetSDLTexture(), { 18, 18 });
				ImGui::PopStyleColor();
			}

			else
			{
				if (ImGui::ImageButton(ProtoContent.GetTexture("Engine/align-right.png")->GetSDLTexture(), { 18, 18 }))
					hAlignment = HAlignment::RIGHT;
			}
		}

		ImGui::SameLine(250);

		/* Vertical Alignment */ {
			if (vAlignment == VAlignment::BOTTOM)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, selectedCol);
				ImGui::ImageButton(Proto::ContentManager::GetInstance().GetTexture("Engine/align-bottom.png")->GetSDLTexture(), { 18, 18 });
				ImGui::PopStyleColor();
			}

			else
			{
				if (ImGui::ImageButton(ProtoContent.GetTexture("Engine/align-bottom.png")->GetSDLTexture(), { 18, 18 }))
					vAlignment = VAlignment::BOTTOM;
			}

			ImGui::SameLine();

			if (vAlignment == VAlignment::CENTER)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, selectedCol);
				ImGui::ImageButton(ProtoContent.GetTexture("Engine/align-middle.png")->GetSDLTexture(), { 18, 18 });
				ImGui::PopStyleColor();
			}

			else
			{
				if (ImGui::ImageButton(ProtoContent.GetTexture("Engine/align-middle.png")->GetSDLTexture(), { 18, 18 }))
					vAlignment = VAlignment::CENTER;
			}

			ImGui::SameLine();

			if (vAlignment == VAlignment::TOP)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, selectedCol);
				ImGui::ImageButton(ProtoContent.GetTexture("Engine/align-top.png")->GetSDLTexture(), { 18, 18 });
				ImGui::PopStyleColor();
			}

			else
			{
				if (ImGui::ImageButton(ProtoContent.GetTexture("Engine/align-top.png")->GetSDLTexture(), { 18, 18 }))
					vAlignment = VAlignment::TOP;
			}
		}
	}
}

void ImGuiProto::Color(const std::string& text, const TextureData& texData, SDL_Color& color, const std::string& address)
{
	ImGui::Text(text.c_str());
	ImGui::SameLine(100);

	const std::string labelText{ "##COLOR" + address };

	float colors[4] = { float(texData.color.r) / 255.f, float(texData.color.g) / 255.f, float(texData.color.b) / 255.f, float(texData.color.a) / 255.f };
	ImGui::PushItemWidth(250);
	ImGui::ColorEdit4(labelText.c_str(), &colors[0]);
	color = { Uint8(colors[0] * 255), Uint8(colors[1] * 255), Uint8(colors[2] * 255), Uint8(colors[3] * 255) };
	ImGui::PopItemWidth();
}


// Derived from https://github.com/ocornut/imgui/issues/1496
void ImGuiProto::BeginGroupPanel(const char* name, const ImVec2& size)
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

void ImGuiProto::EndGroupPanel()
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

void ImGuiProto::BeginComponentPanel(const ImVec2& size, BaseComponent* pCurrComp, BaseComponent** pDelComp, const std::string& address)
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

	std::string labelText;
	if (!dynamic_cast<TransformComponent*>(pCurrComp))
	{
		ImGui::SameLine(270);

		labelText = "^##" + address;
		if (ImGui::Button(labelText.c_str(), { 20, 20 }))
			pCurrComp->GetGameObject()->SwapUpComponent(pCurrComp);

		ImGui::SameLine(300);

		labelText = "v##" + address;
		if (ImGui::Button(labelText.c_str(), { 20, 20 }))
			pCurrComp->GetGameObject()->SwapDownComponent(pCurrComp);

		ImGui::SameLine(330);

		labelText = "X##" + address;
		if (ImGui::Button(labelText.c_str(), { 20, 20 }))
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

void ImGuiProto::EndComponentPanel()
{
	EndGroupPanel();
}
