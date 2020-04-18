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

void ImageComponent::SetAlignment(const HAlignment& horAlignment, const VAlignment& verAlignment)
{
	m_HorAlignment = horAlignment;
	m_VerAlignment = verAlignment;
}

void ImageComponent::Draw()
{
	if (!m_pTexture)
		return;

	RenderData data;
	data.position.x = m_pGameObject->GetTransform()->GetPosition().x + m_TexData.x;
	data.position.y = m_pGameObject->GetTransform()->GetPosition().y + m_TexData.y;
	data.size.x = m_pGameObject->GetTransform()->GetScale().x * m_TexData.width;
	data.size.y = m_pGameObject->GetTransform()->GetScale().y * m_TexData.height;
	data.rotationCenter = m_pGameObject->GetTransform()->GetRotCenter();
	data.angle = m_pGameObject->GetTransform()->GetRotAngle();
	data.color = m_TexData.color;

	switch(m_HorAlignment)
	{
	case HAlignment::LEFT:
		break;
	case HAlignment::CENTER:
		data.position.x -= m_TexData.width / 2.f;
		break;
	case HAlignment::RIGHT:
		data.position.x -= m_TexData.width;
		break;
	default:
		break;
	}

	switch(m_VerAlignment)
	{
	case VAlignment::TOP:
		data.position.y -= m_TexData.height;
		break;
	case VAlignment::CENTER:
		data.position.y -= m_TexData.height / 2.f;
		break;
	case VAlignment::BOTTOM:
		break;
	default:
		break;
	}
	
	ProtoRenderer.RenderTexture(*m_pTexture, data);
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

void ImageComponent::Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent)
{
	using namespace rapidxml;

	xml_node<>* component = doc.allocate_node(node_element, "ImageComponent");

	// Image
	component->append_attribute(doc.allocate_attribute("ImageLocation", doc.allocate_string(m_TexRelPath.c_str())));

	// Texture Data related
	component->append_attribute(doc.allocate_attribute("TexDataX", doc.allocate_string(ToCString(m_TexData.x))));
	component->append_attribute(doc.allocate_attribute("TexDataY", doc.allocate_string(ToCString(m_TexData.y))));
	component->append_attribute(doc.allocate_attribute("TexDataW", doc.allocate_string(ToCString(m_TexData.width))));
	component->append_attribute(doc.allocate_attribute("TexDataH", doc.allocate_string(ToCString(m_TexData.height))));
	component->append_attribute(doc.allocate_attribute("TexDataColorR", doc.allocate_string(ToCString(m_TexData.color.r))));
	component->append_attribute(doc.allocate_attribute("TexDataColorG", doc.allocate_string(ToCString(m_TexData.color.g))));
	component->append_attribute(doc.allocate_attribute("TexDataColorB", doc.allocate_string(ToCString(m_TexData.color.b))));
	component->append_attribute(doc.allocate_attribute("TexDataColorA", doc.allocate_string(ToCString(m_TexData.color.a))));

	// Alignment related
	std::string hAlignmentStr, vAlignmentStr;
	switch (m_HorAlignment)
	{
	case HAlignment::LEFT:		hAlignmentStr = "Left"; break;
	case HAlignment::CENTER:	hAlignmentStr = "Center"; break;
	case HAlignment::RIGHT:		hAlignmentStr = "Right"; break;
	default:;
	}

	switch (m_VerAlignment)
	{
	case VAlignment::TOP:		vAlignmentStr = "Top"; break;
	case VAlignment::CENTER:	vAlignmentStr = "Center"; break;
	case VAlignment::BOTTOM:	vAlignmentStr = "Bottom"; break;
	default:;
	}

	component->append_attribute(doc.allocate_attribute("HorizontalAlignment", doc.allocate_string(hAlignmentStr.c_str())));
	component->append_attribute(doc.allocate_attribute("VerticalAlignment", doc.allocate_string(vAlignmentStr.c_str())));

	pParent->append_node(component);
}
