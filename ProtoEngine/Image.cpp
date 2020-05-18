#include "ProtoEnginePCH.h"
#include "Image.h"

#include "Texture2D.h"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

Image::Image(ComponentID ID, bool isActive, Proto::Texture2D* pTexture, const TextureData& texData)
	: BaseBehaviour(ID, isActive), m_pTexture(pTexture), m_TexData(texData)
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

Image::~Image()
{
	
}

void Image::SetTexture(const std::string& path)
{
	m_pTexture = ProtoContent.GetTexture(path);
	m_TexRelPath = path;

	int width, height;
	SDL_QueryTexture(m_pTexture->GetSDLTexture(), nullptr, nullptr, &width, &height);
	m_TexData.width = float(width);
	m_TexData.height = float(height);
}

void Image::SetTextureData(const TextureData& texData)
{
	m_TexData = texData;
}

void Image::SetAlignment(const HAlignment& horAlignment, const VAlignment& verAlignment)
{
	m_HorAlignment = horAlignment;
	m_VerAlignment = verAlignment;
}

void Image::Draw()
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

void Image::DrawInspectorTitle()
{
	DrawActiveCheckbox();
	ImGui::Text("Image");
}

void Image::DrawInspector()
{
	/* Image */ {
		ImGui::Text("Image");
		ImGui::SameLine(100);

		ImGui::PushItemWidth(175);
		ImGui::InputText("##RENDER_COMP_FILE_PATH", &m_TexRelPath[0], 300, ImGuiInputTextFlags_ReadOnly);
		ImGui::PopItemWidth();

		ImGui::SameLine(285);
		if (ImGui::Button("...##RENDER_COMP_FILE_PATH_BUTTON"))
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

	ProtoGui::Presets::Position(m_TexData.x, m_TexData.y);
	ProtoGui::Presets::Size(m_TexData.width, m_TexData.height);

	ImGui::PushID(this);
	ProtoGui::Presets::Alignment(m_HorAlignment, m_VerAlignment);
	ImGui::PopID();

	ProtoGui::Presets::Color("Color", m_TexData, m_TexData.color);
}

void Image::Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent)
{
	using namespace rapidxml;

	xml_node<>* pComp = doc.allocate_node(node_element, "ImageBehaviour");

	SaveID(doc, pComp);
	SaveActive(doc, pComp);
	
	// Image
	ProtoSaver::XML::SaveString("ImageLocation", m_TexRelPath, doc, pComp);

	// Texture Data related
	ProtoSaver::XML::Save<float>("TexDataX", m_TexData.x, doc, pComp);
	ProtoSaver::XML::Save<float>("TexDataY", m_TexData.y, doc, pComp);
	ProtoSaver::XML::Save<float>("TexDataW", m_TexData.width, doc, pComp);
	ProtoSaver::XML::Save<float>("TexDataH", m_TexData.height, doc, pComp);

	ProtoSaver::XML::Save<Uint8>("TexDataColorR", m_TexData.color.r, doc, pComp);
	ProtoSaver::XML::Save<Uint8>("TexDataColorG", m_TexData.color.g, doc, pComp);
	ProtoSaver::XML::Save<Uint8>("TexDataColorB", m_TexData.color.b, doc, pComp);
	ProtoSaver::XML::Save<Uint8>("TexDataColorA", m_TexData.color.a, doc, pComp);

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

	ProtoSaver::XML::SaveString("HorizontalAlignment", hAlignmentStr, doc, pComp);
	ProtoSaver::XML::SaveString("VerticalAlignment", vAlignmentStr, doc, pComp);

	pParent->append_node(pComp);
}
