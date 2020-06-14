#include "ProtoEnginePCH.h"
#include "AnimatedSprite.h"

#include "Texture2D.h"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

void Animation::Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent)
{
	using namespace rapidxml;
	
	xml_node<>* animationNode = doc.allocate_node(node_element, doc.allocate_string(ProtoConvert::ToString<Animation>().c_str()));
	ProtoSaver::XML::Save<float>("FrameTime", frameTime, doc, animationNode);

	for(TextureFrame& frame : frames)
		frame.Save(doc, animationNode);
	
	pParent->append_node(animationNode);
}

Animation Animation::Load(rapidxml::xml_node<>* pComp)
{
	Animation animation;
	animation.frameTime = ProtoParser::XML::Parse<float>(pComp, "FrameTime");

	for (rapidxml::xml_node<>* pFrameNode = pComp->first_node(ProtoConvert::ToString<TextureFrame>().c_str()); pFrameNode; pFrameNode = pFrameNode->next_sibling(ProtoConvert::ToString<TextureFrame>().c_str()))
		animation.frames.push_back(TextureFrame::Load(pFrameNode));

	return animation;
}

AnimatedSprite::AnimatedSprite(ComponentID ID, bool isActive, int startingAnimation, Proto::Texture2D* pTexture)
	: BaseBehaviour(ID, isActive), m_pTexture(pTexture), m_StartingAnimation(startingAnimation), m_PlayingAnimation(startingAnimation)
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

AnimatedSprite::~AnimatedSprite()
{
	SafeDelete(m_pTexturePreview);
	SafeDelete(m_pSpritePreview);
}

void AnimatedSprite::SetTexture(const std::string& path)
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

void AnimatedSprite::AddAnimation(const Animation& animation)
{
	m_Animations.push_back(animation);
}

void AnimatedSprite::PlayAnimation(unsigned int index, bool paused)
{
	if (index >= m_Animations.size())
		return;

	m_PlayingAnimation = index;
	
	m_CurrentFrame = 0;
	m_pCurrentAnimation = &m_Animations[index];
	m_pCurrentPreviewAnimation = &m_Animations[index];
	m_IsPaused = paused;
}

void AnimatedSprite::Draw()
{
	if (!m_pTexture)
		return;

	if (!m_pCurrentAnimation)
		return;

	if (m_CurrentFrame >= m_pCurrentAnimation->frames.size())
		return;

	if (m_pCurrentAnimation->frames.empty())
		return;

	FrameRenderData data;
	data.originPosition = m_pGameObject->GetTransform()->GetPosition();
	data.originScale = m_pGameObject->GetTransform()->GetScale();
	data.originAngle = m_pGameObject->GetTransform()->GetRotation();

	data.frame = m_pCurrentAnimation->frames[m_CurrentFrame];

	ProtoRenderer.RenderSprite(*m_pTexture, data);
}

void AnimatedSprite::Update()
{
	if (m_IsPaused)
		return;
	
	if (m_pCurrentAnimation)
	{
		m_CurrentTime += ProtoTime.DeltaTime;
		if (m_CurrentTime >= m_pCurrentAnimation->frameTime)
		{
			m_CurrentTime = 0;
			
			if (m_CurrentFrame + 1 < m_pCurrentAnimation->frames.size())
				m_CurrentFrame++;
			else
				m_CurrentFrame = 0;
		}
	}
}

void AnimatedSprite::UpdateUnscaled()
{
	if (m_IsPaused)
		return;
	
	if (m_pCurrentPreviewAnimation)
	{
		m_CurrentPreviewTime += ProtoTime.DeltaTime_Unscaled;
		if (m_CurrentPreviewTime >= m_pCurrentPreviewAnimation->frameTime)
		{
			m_CurrentPreviewTime = 0;

			if (m_CurrentPreviewFrame + 1 < m_pCurrentPreviewAnimation->frames.size())
				m_CurrentPreviewFrame++;
			else
				m_CurrentPreviewFrame = 0;
		}
	}
}

void AnimatedSprite::DrawInspectorTitle()
{
	DrawActiveCheckbox();
	ImGui::Text("Animated Sprite");
}

void AnimatedSprite::DrawInspector()
{
	const std::string fullPath = fs::canonical(ProtoContent.GetDataPath()).string();
	const auto selection = ProtoGui::Presets::Path("Anim. Sprite", 100, m_TexRelPath, "Select a Sprite", fullPath, { "Sprite Files", "*.png *.jpg *.jpeg *.PNG" });;

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
	if (ImGui::Button("Open Animated Sprite Editor", { 300, 25 }))
		ImGui::OpenPopup("Animated Sprite Editor");


#pragma region Animated Sprite Editor
	bool opened = true;
	ImGui::SetNextWindowSize({ 875, 820 });
	ImGui::SetNextWindowPos({ 50, 50 }, ImGuiCond_Appearing);
	if (ImGui::BeginPopupModal("Animated Sprite Editor", &opened, ImGuiWindowFlags_NoResize))
	{
		ImGui::Columns(2, nullptr, false);

		ImGui::SetColumnWidth(0, 360);
		ImGui::SetNextWindowPos({ 60, 60 }, ImGuiCond_Appearing);
		if (ImGui::BeginChild("##EditFrames", { 350, 785 }, true))
		{

			if (ImGui::CollapsingHeader("Preview / Editor Settings"))
			{
				ProtoGui::Presets::InputXY({ "Scale", "X", "Y" }, m_SpritePreviewScale.x, m_SpritePreviewScale.y, { 0.01f, 0, 0, "%.2f" }, 0);

				const ProtoGui::ProtoGuiData headerPGData{ true, 100, -1, true, 70 };
				ProtoGui::DragData headerDragData{ 1.f, 0, 0, "%.0f" };
				ProtoGui::Drag<float>("Rotation", headerPGData, "##SpritePreviewRotation", m_SpritePreviewRotation, headerDragData);

				headerDragData.v_max = float(int(m_Animations.size()) - 1);
				ProtoGui::Drag<int>("Start. Anim.", headerPGData, "##SpriteStartingAnim", m_StartingAnimation, headerDragData);

				ImGui::Spacing();
				ImGui::SameLine(5);
				if (ImGui::Button("(Un)Pause Animation", { 340, 25 }))
					m_IsPaused = !m_IsPaused;
				
				ImGui::Spacing();
				ImGui::SameLine(5);
				if (ImGui::Button("Add Animation", { 340, 25 }))
					AddAnimation({ 0.2f, {} });

				ImGui::Spacing();
				ImGui::SameLine(5);
				if (ImGui::Button("Load Animations", { 340, 25 }))
				{
					const auto loadAnimSelection = pfd::open_file("Select an Animation File", fullPath, { "ProtoAnimations", "*.protoanimations" }).result();
					if (!loadAnimSelection.empty())
					{
						const std::string filePath{ loadAnimSelection[0].substr(fullPath.size()) };

						LoadAnimations(filePath);
					}
				}

				ImGui::Spacing();
				ImGui::SameLine(5);
				if (ImGui::Button("Save Animations", { 340, 25 }))
					ImGui::OpenPopup("Save Animations");

				if (ImGui::BeginPopupModal("Save Animations", NULL, ImGuiWindowFlags_AlwaysAutoResize))
				{
					ImGui::Text("Please fill in a name and select a folder to save these animations.");
					ImGui::Separator();

					m_SaveFileName.resize(100);
					ImGui::Text("File Name: ");
					ImGui::SameLine();
					ImGui::InputText("##SAVE_FILE_NAME", &m_SaveFileName[0], 100);

					ImGui::SameLine();
					ImGui::Text(".protoanimations");

					m_SaveFileFolderStructure.resize(300);
					ImGui::Text("File Location: ");
					ImGui::SameLine();
					ImGui::InputText("##SAVE_FILE_PATH", &m_SaveFileFolderStructure[0], 300, ImGuiInputTextFlags_ReadOnly);
					ImGui::SameLine();

					if (ImGui::Button("...##SAVE_FILE_BUTTON"))
					{
						const auto saveFolderSelection = pfd::select_folder("Select a Folder", fullPath).result();
						if (!saveFolderSelection.empty())
						{
							m_SaveFileFolderStructure = saveFolderSelection.substr(fullPath.size());
						}
					}

					if (m_SaveFileFolderStructure[0] == '\0' || m_SaveFileName[0] == '\0')
					{
						ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
						ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
					}

					if (ImGui::Button("OK", ImVec2(150, 0)))
					{
						SaveAnimations();

						ImGui::CloseCurrentPopup();
					}

					if (m_SaveFileFolderStructure[0] == '\0' || m_SaveFileName[0] == '\0')
					{
						ImGui::PopItemFlag();
						ImGui::PopStyleVar();
					}

					ImGui::SetItemDefaultFocus();
					ImGui::SameLine();

					if (ImGui::Button("Cancel", ImVec2(150, 0)))
						ImGui::CloseCurrentPopup();

					ImGui::EndPopup();
				}
			}

			ImGui::Separator();

#pragma region Animation List
			int toRemoveAnimation{ -1 };
			for (int i{}; i < int(m_Animations.size()); i++)
			{
				if (ImGui::CollapsingHeader(std::string("Animation " + std::to_string(i)).c_str()))
				{
					Animation& animation = m_Animations[i];
					ImGui::PushID(&animation);
					
					ImGui::Text(std::string("Animation " + std::to_string(i)).c_str());
					ImGui::Separator();

					const ProtoGui::ProtoGuiData pgData{ true, 100, -1, true, 70 };
					const ProtoGui::DragData dragData{ 0.01f, 0, 0, "%.2f" };
					ProtoGui::Drag<float>("Frame Time", pgData, "##SpriteAnimationFrameTime", animation.frameTime, dragData);

					ImGui::Spacing();
					ImGui::SameLine(5);
					if (ImGui::Button("Add Frame", { 340, 25 }))
					{
						if (animation.frames.empty())
						{
							TextureFrame newFrame;
							newFrame.dstPosition = { 0, 0 };
							newFrame.dstSize = m_TextureSize;
							newFrame.srcPosition = { 0, 0 };
							newFrame.srcSize = m_TextureSize;

							newFrame.scale = { 1, 1 };
							newFrame.pivot = { 0, 0 };

							newFrame.color = { 255, 255, 255, 255 };
							animation.frames.push_back(newFrame);
						}

						else
						{
							animation.frames.push_back(animation.frames.back());
						}
					}

					ImGui::Spacing();
					ImGui::SameLine(5);
					if (ImGui::Button("Preview Animation", { 340, 25 }))
					{
						m_CurrentFrame = 0;
						m_CurrentPreviewFrame = 0;
						m_pCurrentPreviewAnimation = &animation;
						m_pCurrentAnimation = &animation;
					}

					ImGui::Spacing();
					ImGui::SameLine(5);
					if (ImGui::Button("Delete Animation", { 340, 25 }))
						toRemoveAnimation = i;

					ImGui::Separator();

					int toRemoveFrame{ -1 };
					for (int j{}; j < int(animation.frames.size()); j++)
					{
						TextureFrame& frame = animation.frames[j];

						ImGui::Indent(20);
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
						ImGui::Indent(-20);
					}

					if (toRemoveFrame != -1)
						animation.frames.erase(animation.frames.begin() + toRemoveFrame);

					ImGui::PopID();
				}
			}

			if (toRemoveAnimation != -1)
			{
				if (m_pCurrentPreviewAnimation == &m_Animations[toRemoveAnimation] || m_pCurrentAnimation == &m_Animations[toRemoveAnimation])
					m_pCurrentPreviewAnimation = m_pCurrentAnimation = nullptr;
				
				m_Animations.erase(m_Animations.begin() + toRemoveAnimation);
			}
		}
#pragma endregion Animation List

		ImGui::EndChild();

		ImGui::NextColumn();

#pragma region Texture Preview
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

				if (m_pCurrentPreviewAnimation)
				{
					for (auto& frame : m_pCurrentPreviewAnimation->frames)
					{
						SDL_Rect src;
						src.x = int(frame.srcPosition.x);
						src.y = int(frame.srcPosition.y);
						src.w = int(frame.srcSize.x);
						src.h = int(frame.srcSize.y);
						ProtoRenderer.RenderLineRect(src, { 255, 255, 255, 255 });
					}
				}
				
				SDL_SetRenderTarget(ProtoRenderer.GetSDLRenderer(), nullptr);
			}

			if (previewSize.x > maxPreviewSize.x)
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
#pragma endregion Texture Preview

#pragma region Sprite Preview
		ImGui::SetNextWindowPos({ 60 + 360, 60 + 400 }, ImGuiCond_Appearing);
		if (ImGui::BeginChild("##SpritePreview", { 490, 390 }, true))
		{
			const glm::vec2 previewSize{ 475, 375 };

		#pragma region Render Target Draw
			ProtoSettings.SetEditorRenderMode(RenderMode::GAME);
			SDL_SetRenderTarget(ProtoRenderer.GetSDLRenderer(), m_pSpritePreview->GetSDLTexture());
			SDL_RenderClear(ProtoRenderer.GetSDLRenderer());

			if(m_pCurrentPreviewAnimation && !m_pCurrentPreviewAnimation->frames.empty())
			{
				FrameRenderData data;
				data.originPosition = previewSize / 2.f;
				data.originScale = m_SpritePreviewScale;
				data.originAngle = m_SpritePreviewRotation;
				data.frame = m_pCurrentPreviewAnimation->frames[m_CurrentPreviewFrame];
				
				ProtoRenderer.RenderSprite(*m_pTexture, data);
			}
			
			ProtoRenderer.RenderLine({ 0, previewSize.y / 2.f }, { previewSize.x, previewSize.y / 2.f }, { 255, 255, 255, 50 });
			ProtoRenderer.RenderLine({ previewSize.x / 2.f, 0 }, { previewSize.x / 2.f, previewSize.y }, { 255, 255, 255, 50 });

			SDL_SetRenderTarget(ProtoRenderer.GetSDLRenderer(), nullptr);
		#pragma endregion Render Target Draw

			ImVec2 cursorPos;
			cursorPos.x += 5;
			cursorPos.y += 5;
			ImGui::SetCursorPos(cursorPos);

			ImGui::Image(m_pSpritePreview->GetSDLTexture(), ProtoConvert::ToImGuiVec(previewSize));
		}
		ImGui::EndChild();
#pragma endregion Sprite Preview
		
		ImGui::Columns(1);
		ImGui::EndPopup();
	}

#pragma endregion Animated Sprite Editor
}

void AnimatedSprite::Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent)
{
	using namespace rapidxml;
	
	xml_node<>* pComp = doc.allocate_node(node_element, doc.allocate_string(ProtoConvert::ToString<AnimatedSprite>().c_str()));
	SaveID(doc, pComp);
	SaveActive(doc, pComp);
	
	ProtoSaver::XML::SaveString("SpriteLocation", m_TexRelPath, doc, pComp);
	ProtoSaver::XML::Save<int>("StartingAnimation", m_StartingAnimation, doc, pComp);
	
	for (Animation& animation : m_Animations)
		animation.Save(doc, pComp);
	
	pParent->append_node(pComp);
}

void AnimatedSprite::Load(rapidxml::xml_node<>* pComp, GameObject* pCurr)
{
	const auto id{ ProtoParser::XML::Parse<unsigned int>(pComp, "ID") };
	const auto isActive{ ProtoParser::XML::Parse<bool>(pComp, "Active") };
	
	const std::string spriteLocation{ ProtoParser::XML::ParseString(pComp, "SpriteLocation") };
	const int startingAnim{ ProtoParser::XML::Parse<int>(pComp, "StartingAnimation") };

	Proto::Texture2D* pTexture{ spriteLocation.empty() ? nullptr : ProtoContent.GetTexture(spriteLocation) };
	const auto pSpriteComp = new AnimatedSprite(id, isActive, startingAnim, pTexture);
	pCurr->AddComponent(pSpriteComp);
	
	for (rapidxml::xml_node<>* pAnimNode = pComp->first_node(ProtoConvert::ToString<Animation>().c_str()); pAnimNode; pAnimNode = pAnimNode->next_sibling(ProtoConvert::ToString<Animation>().c_str()))
		pSpriteComp->AddAnimation(Animation::Load(pAnimNode));

	pSpriteComp->PlayAnimation(startingAnim, false);
}



void AnimatedSprite::LoadAnimations(const std::string& filePath)
{	
	using namespace rapidxml;
	
	xml_document<> doc;
	const std::string path{ ProtoContent.GetDataPath() + filePath };
	std::ifstream file(path);

	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string content(buffer.str());

	doc.parse<0>(&content[0]);

	xml_node<>* scene = doc.first_node("Animations");

	for (xml_node<>* animNode = scene->first_node(ProtoConvert::ToString<Animation>().c_str()); animNode; animNode = animNode->next_sibling(ProtoConvert::ToString<Animation>().c_str()))
		m_Animations.push_back(Animation::Load(animNode));
}

void AnimatedSprite::SaveAnimations()
{
	using namespace rapidxml;

	xml_document<> doc;
	/* Basic Node Declaration - Version + Encoding */ {
		xml_node<>* decl = doc.allocate_node(node_declaration);
		decl->append_attribute(doc.allocate_attribute("version", "1.0"));
		decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
		doc.append_node(decl);
	}

	xml_node<>* animations = doc.allocate_node(node_element, "Animations");
	animations->append_attribute(doc.allocate_attribute("version", "1.0"));
	animations->append_attribute(doc.allocate_attribute("type", "protoanimations"));

	for (auto anim : m_Animations)
		anim.Save(doc, animations);

	doc.append_node(animations);

	const std::string path{ ProtoContent.GetDataPath() + m_SaveFileFolderStructure.c_str() + "\\" + m_SaveFileName.c_str() + ".protoanimations" };  // NOLINT(readability-redundant-string-cstr)
																														// c_str() is necessary as strings have reserved space (filled with '\0')
	fs::create_directory(ProtoContent.GetDataPath() + m_SaveFileFolderStructure.c_str());  // NOLINT(readability-redundant-string-cstr)
	std::ofstream fileStored(path);
	fileStored << doc;
	fileStored.close();
	doc.clear();
}