#include "ProtoEnginePCH.h"
#include "ImGui_Proto.h"

#include "ContentManager.h"
#include "Font.h"
#include "Texture2D.h"
#include "ImGui/imgui.h"

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
	ImGui::ColorEdit4(labelText.c_str(), &colors[0]);
	color = { Uint8(colors[0] * 255), Uint8(colors[1] * 255), Uint8(colors[2] * 255), Uint8(colors[3] * 255) };
}