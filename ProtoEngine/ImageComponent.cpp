#include "ProtoEnginePCH.h"
#include "ImageComponent.h"

#include "Texture2D.h"
#include "Utils.h"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

ImageComponent::ImageComponent(Proto::Texture2D* pTexture, const TextureData& texData)
	: m_pTexture(pTexture), m_TexData(texData)
{
	if (m_pTexture)
	{
		m_TexRelPath = pTexture->GetRelativePath();

		int width, height;
		SDL_QueryTexture(m_pTexture->GetSDLTexture(), nullptr, nullptr, &width, &height);
		m_TexData.width = float(width);
		m_TexData.height = float(height);
	}
}

ImageComponent::~ImageComponent()
{
	
}

void ImageComponent::SetTexture(const std::string& path)
{
	m_pTexture = ProtoContent.GetTexture(path);
	m_TexRelPath = path;

	int width, height;
	SDL_QueryTexture(m_pTexture->GetSDLTexture(), nullptr, nullptr, &width, &height);
	m_TexData.width = float(width);
	m_TexData.height = float(height);
}

void ImageComponent::SetTextureData(const TextureData& texData)
{
	m_TexData = texData;
}

void ImageComponent::Initialize()
{
}

void ImageComponent::Update()
{
}

void ImageComponent::FixedUpdate()
{
}

void ImageComponent::Draw()
{
	if (!m_pTexture)
		return;

	float x{ m_pGameObject->GetTransform()->GetPosition().x + m_TexData.x }, y{ m_pGameObject->GetTransform()->GetPosition().y + m_TexData.y };

	switch(m_HorAlignment)
	{
	case HAlignment::LEFT:
		break;
	case HAlignment::CENTER:
		x -= m_TexData.width / 2.f;
		break;
	case HAlignment::RIGHT:
		x -= m_TexData.width;
		break;
	default:
		break;
	}

	switch(m_VerAlignment)
	{
	case VAlignment::TOP:
		y -= m_TexData.height;
		break;
	case VAlignment::CENTER:
		y -= m_TexData.height / 2.f;
		break;
	case VAlignment::BOTTOM:
		break;
	default:
		break;
	}
	
	ProtoRenderer.RenderTexture(*m_pTexture, x, y, 
		m_pGameObject->GetTransform()->GetScale().x * m_TexData.width,
		m_pGameObject->GetTransform()->GetScale().y * m_TexData.height, m_TexData.color);
}

void ImageComponent::DrawInspectorTitle()
{
	ImGui::Text("Image");
}

void ImageComponent::DrawInspector()
{
	std::stringstream thisAddress;
	thisAddress << this;

	std::string labelText;

	/* Image */ {
		ImGui::Text("Image");
		ImGui::SameLine(100);

		ImGui::PushItemWidth(175);
		labelText = "##RENDER_COMP_FILE_PATH" + thisAddress.str();
		ImGui::InputText(labelText.c_str(), &m_TexRelPath[0], 300, ImGuiInputTextFlags_ReadOnly);
		ImGui::PopItemWidth();

		ImGui::SameLine(285);
		labelText = "...##RENDER_COMP_FILE_PATH_BUTTON" + thisAddress.str();
		if (ImGui::Button(labelText.c_str()))
		{
			const std::string fullPath = fs::canonical(ProtoContent.GetDataPath()).string();
			const auto selection = pfd::open_file("Select an Image", fullPath, { "Image Files", "*.png *.jpg *.jpeg *.PNG" }).result();
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
		}
	}

	if (!m_pTexture)
		return;

	ImGuiProto::Position(m_TexData, thisAddress.str());
	ImGuiProto::Size(m_TexData, thisAddress.str());

	ImGui::PushID(this);
	ImGuiProto::Alignment(m_HorAlignment, m_VerAlignment);
	ImGui::PopID();

	ImGuiProto::Color("Color", m_TexData, m_TexData.color, thisAddress.str());
}