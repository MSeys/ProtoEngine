#include "ProtoEnginePCH.h"
#include "RenderComponent.h"

#include "Texture2D.h"
#include "Utils.h"

RenderComponent::RenderComponent(Proto::Texture2D* pTexture, const Proto::TextureData& texData)
	: m_pTexture(pTexture), m_TexData(texData)
{
	if (m_pTexture)
		m_TexRelPath = pTexture->GetRelativePath();
}

RenderComponent::~RenderComponent()
{
	SafeDelete(m_pTexture);
}

void RenderComponent::SetTexture(const std::string& path)
{
	SafeDelete(m_pTexture);
	m_pTexture = ProtoResources.LoadTexture(path);
	m_TexRelPath = path;
}

void RenderComponent::SetTextureData(const Proto::TextureData& texData)
{
	m_TexData = texData;
}

void RenderComponent::Initialize()
{
}

void RenderComponent::Update()
{
}

void RenderComponent::FixedUpdate()
{
}

void RenderComponent::Draw()
{
	if (!m_pTexture)
		return;
	
	if(m_TexData.width == -1 || m_TexData.height == -1)
		ProtoRenderer.RenderTexture(*m_pTexture, m_pGameObject->GetTransform()->GetPosition().x + m_TexData.x, m_pGameObject->GetTransform()->GetPosition().y + m_TexData.y, m_TexData.color);

	else
		ProtoRenderer.RenderTexture(*m_pTexture, m_pGameObject->GetTransform()->GetPosition().x + m_TexData.x, m_pGameObject->GetTransform()->GetPosition().y + m_TexData.y, m_TexData.width, m_TexData.height, m_TexData.color);
}

void RenderComponent::DrawInspectorTitle()
{
	ImGui::Text("Render Component");
}

void RenderComponent::DrawInspector()
{
	std::stringstream thisAddress;
	thisAddress << this;

	std::string labelText{};

	/* Position */ {
		ImGui::Text("Position");

		ImGui::SameLine(100);
		ImGui::Text("X");
		ImGui::SameLine(115);

		ImGui::PushItemWidth(70);
		labelText = "##RENDER_COMP_POS_X" + thisAddress.str();
		ImGui::InputFloat(labelText.c_str(), &m_TexData.x, 0, 0, "%.1f");
		ImGui::PopItemWidth();

		ImGui::SameLine(200);
		ImGui::Text("Y");
		ImGui::SameLine(215);

		ImGui::PushItemWidth(70);
		labelText = "##RENDER_COMP_POS_Y" + thisAddress.str();
		ImGui::InputFloat(labelText.c_str(), &m_TexData.y, 0, 0, "%.1f");
		ImGui::PopItemWidth();
	}
	
	/* Size */ {
		ImGui::Text("Size");

		ImGui::SameLine(100);
		ImGui::Text("W");
		ImGui::SameLine(115);

		ImGui::PushItemWidth(70);
		labelText = "##RENDER_COMP_SIZE_WIDTH" + thisAddress.str();
		ImGui::InputFloat(labelText.c_str(), &m_TexData.width, 0, 0, "%.1f");
		ImGui::PopItemWidth();

		ImGui::SameLine(200);
		ImGui::Text("H");
		ImGui::SameLine(215);

		ImGui::PushItemWidth(70);
		labelText = "##RENDER_COMP_SIZE_HEIGHT" + thisAddress.str();
		ImGui::InputFloat(labelText.c_str(), &m_TexData.height, 0, 0, "%.1f");
		ImGui::PopItemWidth();
	}

	/* Image */ {
		ImGui::Text("Image");
		ImGui::SameLine(100);

		labelText = "##RENDER_COMP_FILE_PATH" + thisAddress.str();

		m_TexRelPath.resize(150);

		ImGui::InputText(labelText.c_str(), &m_TexRelPath[0], 150);
		if (ProtoResources.FileExists(m_TexRelPath, ResourceType::IMAGE))
		{
			if (m_pTexture)
			{
				if (m_TexRelPath != m_pTexture->GetRelativePath())
					SetTexture(m_TexRelPath);
			}

			else
				SetTexture(m_TexRelPath);
		}
	}

	if (!m_pTexture)
		return;

	/* Color */ {
		ImGui::Text("Color");
		ImGui::SameLine(100);

		labelText = "##RENDER_COMP_IMAGE_COLOR" + thisAddress.str();

		float colors[4] = { float(m_TexData.color.r) / 255.f, float(m_TexData.color.g) / 255.f, float(m_TexData.color.b) / 255.f, float(m_TexData.color.a) / 255.f };
		ImGui::ColorEdit4(labelText.c_str(), &colors[0]);
		m_TexData.color = { Uint8(colors[0] * 255), Uint8(colors[1] * 255), Uint8(colors[2] * 255), Uint8(colors[3] * 255) };
	}
}
