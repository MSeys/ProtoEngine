#include "ProtoEnginePCH.h"
#include "Sprite.h"

#include "Texture2D.h"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

Sprite::Sprite(ComponentID ID, bool isActive, Proto::Texture2D* pTexture)
	: BaseBehaviour(ID, isActive), m_pTexture(pTexture)
{
	SDL_Texture* pSpritePreview{ SDL_CreateTexture(ProtoRenderer.GetSDLRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
										475, 375) };
	m_pSpritePreview = new Proto::Texture2D(pSpritePreview, "", "");
	
	if (!m_pTexture)
		return;

	m_TexRelPath = m_pTexture->GetRelativePath();

	int width, height;
	SDL_QueryTexture(m_pTexture->GetSDLTexture(), nullptr, nullptr, &width, &height);
	m_TextureSize.x = float(width);
	m_TextureSize.y = float(height);

	SDL_Texture* pTexturePreview{ SDL_CreateTexture(ProtoRenderer.GetSDLRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
										width, height) };

	m_pTexturePreview = new Proto::Texture2D(pTexturePreview, "", "");
}

Sprite::~Sprite()
{
	SafeDelete(m_pTexturePreview);
	SafeDelete(m_pSpritePreview);
}

void Sprite::SetTexture(const std::string& path)
{
	m_pTexture = ProtoContent.GetTexture(path);
	m_TexRelPath = path;

	int width, height;
	SDL_QueryTexture(m_pTexture->GetSDLTexture(), nullptr, nullptr, &width, &height);
	m_TextureSize.x = float(width);
	m_TextureSize.y = float(height);

	SafeDelete(m_pTexturePreview);
	SDL_Texture* pTexturePreview{ SDL_CreateTexture(ProtoRenderer.GetSDLRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
										width, height) };

	m_pTexturePreview = new Proto::Texture2D(pTexturePreview, "", "");
}

void Sprite::AddFrame(const TextureFrame& frame)
{
	m_Frames.push_back(frame);
}

void Sprite::Draw()
{
	if (!m_pTexture)
		return;

	for(auto frame : m_Frames)
	{
		FrameRenderData data;
		data.originPosition = m_pGameObject->GetTransform()->GetPosition();
		data.originScale = m_pGameObject->GetTransform()->GetScale();
		data.originAngle = m_pGameObject->GetTransform()->GetRotation();
		data.frame = frame;
		
		ProtoRenderer.RenderSprite(*m_pTexture, data);
	}
}

void Sprite::DrawInspectorTitle()
{
	DrawActiveCheckbox();
	ImGui::Text("Sprite");
}

void Sprite::DrawInspector()
{
	const std::string fullPath = fs::canonical(ProtoContent.GetDataPath()).string();
	const auto selection = ProtoGui::Presets::Path("Sprite", 100, m_TexRelPath, "Select a Sprite", fullPath, { "Sprite Files", "*.png *.jpg *.jpeg *.PNG" });;

	if (!selection.empty())
	{
		const std::string relPath{ selection[0].substr(fullPath.size()) };

		if (m_pTexture)
		{
			if (relPath != m_pTexture->GetRelativePath())
				SetTexture(relPath);
		}

		else
			SetTexture(relPath);
	}

	if (!m_pTexture)
		return;
	
	ImGui::Spacing();
	ImGui::SameLine(25);
	if (ImGui::Button("Open Sprite Editor", { 300, 25 }))
		ImGui::OpenPopup("Sprite Editor");

	bool opened = true;
	ImGui::SetNextWindowSize({ 875, 820 });
	ImGui::SetNextWindowPos({ 50, 50 }, ImGuiCond_Appearing);
	if (ImGui::BeginPopupModal("Sprite Editor", &opened, ImGuiWindowFlags_NoResize))
	{
		ImGui::Columns(2, nullptr, false);

		ImGui::SetColumnWidth(0, 360);
		ImGui::SetNextWindowPos({ 60, 60 }, ImGuiCond_Appearing);
		if(ImGui::BeginChild("##EditFrames", { 350, 785 }, true))
		{
			if (ImGui::CollapsingHeader("Preview / Editor Settings"))
			{
				ProtoGui::Presets::InputXY({ "Scale", "X", "Y" }, m_SpritePreviewScale.x, m_SpritePreviewScale.y, { 0.01f, 0, 0, "%.2f" }, 0);

				const ProtoGui::ProtoGuiData pgData{ true, 100, -1, true, 70 };
				const ProtoGui::DragData dragData{ 1.f, 0, 0, "%.0f" };
				ProtoGui::Drag<float>("Rotation", pgData, "##SpritePreviewRotation", m_SpritePreviewRotation, dragData);

				ImGui::Spacing();
				ImGui::SameLine(5);
				if (ImGui::Button("Add Frame", { 340, 25 }))
				{
					TextureFrame newFrame;
					newFrame.dstPosition = { 0, 0 };
					newFrame.dstSize = m_TextureSize;
					newFrame.srcPosition = { 0, 0 };
					newFrame.srcSize = m_TextureSize;

					newFrame.scale = { 1, 1 };
					newFrame.pivot = { 0, 0 };

					newFrame.color = { 255, 255, 255, 255 };
					m_Frames.push_back(newFrame);
				}
			}
			
			ImGui::Separator();

			int toRemoveFrame{ -1 };
			for (int j{}; j < int(m_Frames.size()); j++)
			{
				TextureFrame& frame = m_Frames[j];

				ImGui::BeginGroup();
				if (ImGui::CollapsingHeader(std::string("Frame " + std::to_string(j)).c_str()))
				{
					ImGui::PushID(&frame);

					ImGui::Spacing();
					ImGui::SameLine(5);
					if (ImGui::Button("Delete", { 310, 25 }))
						toRemoveFrame = j;

					ProtoGui::Presets::InputXY({ "Dst. Position", "X", "Y" }, frame.dstPosition.x, frame.dstPosition.y, { 1.f, 0, 0, "%.0f" }, 0);
					ProtoGui::Presets::InputXY({ "Dst. Size", "X", "Y" }, frame.dstSize.x, frame.dstSize.y, { 1.f, 0, 0, "%.0f" }, 1);
					ProtoGui::Presets::InputXY({ "Dst. Pivot", "X", "Y" }, frame.pivot.x, frame.pivot.y, { 0.1f, 0, 1, "%.1f" }, 2);
					ProtoGui::Presets::InputXY({ "Dst. Scale", "X", "Y" }, frame.scale.x, frame.scale.y, { 0.01f, 0, 0, "%.2f" }, 3);

					ImGui::Separator();
					ProtoGui::Presets::InputXY({ "Src. Position", "X", "Y" }, frame.srcPosition.x, frame.srcPosition.y, { 1.f, 0, 0, "%.0f" }, 4);
					ProtoGui::Presets::InputXY({ "Src. Size", "X", "Y" }, frame.srcSize.x, frame.srcSize.y, { 1.f, 0, 0, "%.0f" }, 5);

					ImGui::Text("Color");
					ImGui::SameLine(100);
					ProtoGui::Presets::Color(frame.color, frame.color, 6);

					ImGui::PopID();
				}
				ImGui::EndGroup();
			}

			if (toRemoveFrame != -1)
				m_Frames.erase(m_Frames.begin() + toRemoveFrame);

		}
		ImGui::EndChild();

		ImGui::NextColumn();
		
		ImGui::SetNextWindowPos({ 60 + 360, 60 }, ImGuiCond_Appearing);
		if (ImGui::BeginChild("##TexturePreview", { 490, 390 }, true))
		{
			const glm::vec2 maxPreviewSize{ 475, 375 };
			glm::vec2 previewSize{ m_TextureSize };
			
			/* RenderTarget Draw */ {
				ProtoSettings.SetEditorRenderMode(RenderMode::GAME);
				SDL_SetRenderTarget(ProtoRenderer.GetSDLRenderer(), m_pTexturePreview->GetSDLTexture());
				SDL_RenderClear(ProtoRenderer.GetSDLRenderer());

				RenderData data;
				data.originPosition = { 0, 0 };
				data.originScale = { 1, 1 };
				data.originAngle = 0;

				data.position = { 0, 0 };
				data.scale = { 1, 1 };
				data.pivot = { 0, 0 };

				data.size = m_TextureSize;
				data.color = { 255, 255, 255, 255 };
				ProtoRenderer.RenderTexture(*m_pTexture, data);

				for (auto& frame : m_Frames)
				{
					SDL_Rect src;
					src.x = int(frame.srcPosition.x);
					src.y = int(frame.srcPosition.y);
					src.w = int(frame.srcSize.x);
					src.h = int(frame.srcSize.y);
					ProtoRenderer.RenderLineRect(src, { 255, 255, 255, 255 });
				}

				SDL_SetRenderTarget(ProtoRenderer.GetSDLRenderer(), nullptr);
			}

			if(previewSize.x > maxPreviewSize.x)
			{
				const float aspectRatio{ maxPreviewSize.x / m_TextureSize.x };
				previewSize = m_TextureSize * aspectRatio;
			}

			if (previewSize.y > maxPreviewSize.y)
			{
				const float aspectRatio{ maxPreviewSize.y / m_TextureSize.y };
				previewSize = m_TextureSize * aspectRatio;
			}

			ImVec2 cursorPos;
			cursorPos = ProtoConvert::ToImGuiVec((maxPreviewSize - previewSize) * 0.5f);
			cursorPos.x += 10;
			cursorPos.y += 10;
			ImGui::SetCursorPos(cursorPos);
			
			ImGui::Image(m_pTexturePreview->GetSDLTexture(), ProtoConvert::ToImGuiVec(previewSize));
		}
		ImGui::EndChild();

		ImGui::SetNextWindowPos({ 60 + 360, 60 + 400 }, ImGuiCond_Appearing);
		if (ImGui::BeginChild("##SpritePreview", { 490, 390 }, true))
		{
			const glm::vec2 previewSize{ 475, 375 };

			/* RenderTarget Draw */ {
				ProtoSettings.SetEditorRenderMode(RenderMode::GAME);
				SDL_SetRenderTarget(ProtoRenderer.GetSDLRenderer(), m_pSpritePreview->GetSDLTexture());
				SDL_RenderClear(ProtoRenderer.GetSDLRenderer());

				for (auto& frame : m_Frames)
				{
					FrameRenderData data;
					data.originPosition = previewSize / 2.f;
					data.originScale = m_SpritePreviewScale;
					data.originAngle = m_SpritePreviewRotation;
					data.frame = frame;
					
					ProtoRenderer.RenderSprite(*m_pTexture, data);
				}

				ProtoRenderer.RenderLine({ 0, previewSize.y / 2.f }, { previewSize.x, previewSize.y / 2.f }, { 255, 255, 255, 50 });
				ProtoRenderer.RenderLine({ previewSize.x / 2.f, 0 }, { previewSize.x / 2.f, previewSize.y }, { 255, 255, 255, 50 });
				
				SDL_SetRenderTarget(ProtoRenderer.GetSDLRenderer(), nullptr);
			}

			ImVec2 cursorPos;
			cursorPos.x += 5;
			cursorPos.y += 5;
			ImGui::SetCursorPos(cursorPos);

			ImGui::Image(m_pSpritePreview->GetSDLTexture(), ProtoConvert::ToImGuiVec(previewSize));
		}
		ImGui::EndChild();

		ImGui::Columns(1);
		
		ImGui::EndPopup();
	}
}

void Sprite::Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent)
{
	using namespace rapidxml;

	xml_node<>* pComp = doc.allocate_node(node_element, doc.allocate_string(ProtoConvert::ToString<Sprite>().c_str()));
	SaveID(doc, pComp);
	SaveActive(doc, pComp);

	ProtoSaver::XML::SaveString("SpriteLocation", m_TexRelPath, doc, pComp);

	for (TextureFrame& frame : m_Frames)
		frame.Save(doc, pComp);
	
	pParent->append_node(pComp);
}

void Sprite::Load(rapidxml::xml_node<>* pComp, GameObject* pCurr)
{
	const auto id{ ProtoParser::XML::Parse<unsigned int>(pComp, "ID") };
	const auto isActive{ ProtoParser::XML::Parse<bool>(pComp, "Active") };

	const std::string spriteLocation{ ProtoParser::XML::ParseString(pComp, "SpriteLocation") };

	Proto::Texture2D* pTexture{ spriteLocation.empty() ? nullptr : ProtoContent.GetTexture(spriteLocation) };
	const auto pSpriteComp = new Sprite(id, isActive, pTexture);
	pCurr->AddComponent(pSpriteComp);

	for (rapidxml::xml_node<>* pFrameNode = pComp->first_node(ProtoConvert::ToString<TextureFrame>().c_str()); pFrameNode; pFrameNode = pFrameNode->next_sibling(ProtoConvert::ToString<TextureFrame>().c_str()))
		pSpriteComp->AddFrame(TextureFrame::Load(pFrameNode));
}