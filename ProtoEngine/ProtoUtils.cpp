#include "ProtoEnginePCH.h"
#include "ProtoUtils.h"

using namespace rapidxml;

void ProtoSaver::XML::SaveString(const char* name, std::string& value, xml_document<>& doc, xml_node<>* pComp)
{
	pComp->append_attribute(doc.allocate_attribute(name, doc.allocate_string(value.c_str())));
}

void ProtoParser::XML::LoadComponents(xml_node<>* pComponents, GameObject* pCurr)
{
	Helper::LoadTransformComponent(pComponents, pCurr);
	Helper::LoadRigidBody2DComponent(pComponents, pCurr);
	Helper::LoadBoxCollider2DComponents(pComponents, pCurr);
	Helper::LoadSphereCollider2DComponents(pComponents, pCurr);
	Helper::LoadLineCollider2DComponents(pComponents, pCurr);
	
	Helper::LoadCameraComponent(pComponents, pCurr);
	Helper::LoadImageComponents(pComponents, pCurr);
	Helper::LoadTextComponents(pComponents, pCurr);
	Helper::LoadFPSComponents(pComponents, pCurr);
	
	if (ProtoSettings.GetRenderMode() == RenderMode::GAME)
		ProtoSettings.GetGame()->LoadComponents(pComponents, pCurr);

	else
		ProtoSettings.GetRefGame()->LoadComponents(pComponents, pCurr);
}

std::string ProtoParser::XML::ParseString(xml_node<>* pComp, const std::string& name)
{
	const auto attribute = pComp->first_attribute(name.c_str());
	if (!attribute)
		return "";
	
	const auto value = attribute->value();
	if (!value)
		return "";

	return std::string(value);
}

void ProtoParser::XML::Helper::LoadTransformComponent(xml_node<>* pComponents, GameObject* pCurr)
{
	xml_node<>* pTransformNode{ pComponents->first_node("Transform") };
	auto* pTransformComp{ pCurr->GetTransform() };

	glm::vec2 position;
	position.x = Parse<float>(pTransformNode, "PositionX");
	position.y = Parse<float>(pTransformNode, "PositionY");

	glm::vec2 rotCenter;
	rotCenter.x = Parse<float>(pTransformNode, "RotCenterX");
	rotCenter.y = Parse<float>(pTransformNode, "RotCenterY");

	const float rotAngle{ Parse<float>(pTransformNode, "RotAngle") };

	glm::vec2 scale;
	scale.x = Parse<float>(pTransformNode, "ScaleX");
	scale.y = Parse<float>(pTransformNode, "ScaleY");

	pTransformComp->SetPosition(position.x, position.y);
	pTransformComp->SetRotCenter(rotCenter.x, rotCenter.y);
	pTransformComp->SetRotAngle(rotAngle);
	pTransformComp->SetScale(scale.x, scale.y);
}

void ProtoParser::XML::Helper::LoadRigidBody2DComponent(xml_node<>* pComponents, GameObject* pCurr)
{
	xml_node<>* pComp{ pComponents->first_node("RigidBody2D") };

	if (!pComp)
		return;
	
	const auto id{ Parse<unsigned int>(pComp, "ID") };
	const auto bodyType{ Parse<int>(pComp, "BodyType") };

	pCurr->AddComponent(new RigidBody2D(id, static_cast<RigidBodyType>(bodyType)));
}

void ProtoParser::XML::Helper::LoadBoxCollider2DComponents(xml_node<>* pComponents, GameObject* pCurr)
{
	for (xml_node<>* pComp = pComponents->first_node("BoxCollider2D"); pComp; pComp = pComp->next_sibling("BoxCollider2D"))
	{
		const auto id{ Parse<unsigned int>(pComp, "ID") };
		const auto isActive{ Parse<bool>(pComp, "Active") };

		const glm::vec2 collisionSize{ Parse<float>(pComp, "CollisionSizeX"), Parse<float>(pComp, "CollisionSizeY") };

		const auto density{ Parse<float>(pComp, "Density") };
		const auto friction{ Parse<float>(pComp, "Friction") };
		const auto restitution{ Parse<float>(pComp, "Restitution") };
		const auto isTrigger{ Parse<bool>(pComp, "IsTrigger") };

		const auto pBoxCollider = new BoxCollider2D(id, isActive, collisionSize, density, friction, restitution, isTrigger);
		pCurr->AddComponent(pBoxCollider);
	}
}

void ProtoParser::XML::Helper::LoadSphereCollider2DComponents(xml_node<>* pComponents, GameObject* pCurr)
{
	for (xml_node<>* pComp = pComponents->first_node("SphereCollider2D"); pComp; pComp = pComp->next_sibling("SphereCollider2D"))
	{
		const auto id{ Parse<unsigned int>(pComp, "ID") };
		const auto isActive{ Parse<bool>(pComp, "Active") };

		const glm::vec2 position{ Parse<float>(pComp, "PositionX"), Parse<float>(pComp, "PositionY") };
		const auto radius{ Parse<float>(pComp, "Radius") };
		
		const auto density{ Parse<float>(pComp, "Density") };
		const auto friction{ Parse<float>(pComp, "Friction") };
		const auto restitution{ Parse<float>(pComp, "Restitution") };
		const auto isTrigger{ Parse<bool>(pComp, "IsTrigger") };

		const auto pSphereCollider = new SphereCollider2D(id, isActive, position, radius, density, friction, restitution, isTrigger);
		pCurr->AddComponent(pSphereCollider);
	}
}

void ProtoParser::XML::Helper::LoadLineCollider2DComponents(xml_node<>* pComponents, GameObject* pCurr)
{
	for (xml_node<>* pComp = pComponents->first_node("LineCollider2D"); pComp; pComp = pComp->next_sibling("LineCollider2D"))
	{
		const auto id{ Parse<unsigned int>(pComp, "ID") };
		const auto isActive{ Parse<bool>(pComp, "Active") };

		const glm::vec2 posA{ Parse<float>(pComp, "PointAX"), Parse<float>(pComp, "PointAY") };
		const glm::vec2 posB{ Parse<float>(pComp, "PointBX"), Parse<float>(pComp, "PointBY") };
		
		const auto density{ Parse<float>(pComp, "Density") };
		const auto friction{ Parse<float>(pComp, "Friction") };
		const auto restitution{ Parse<float>(pComp, "Restitution") };
		const auto isTrigger{ Parse<bool>(pComp, "IsTrigger") };

		const auto pLineCollider = new LineCollider2D(id, isActive, posA, posB, density, friction, restitution, isTrigger);
		pCurr->AddComponent(pLineCollider);
	}
}

void ProtoParser::XML::Helper::LoadImageComponents(xml_node<>* pComponents, GameObject* pCurr)
{
	for (xml_node<>* pComp = pComponents->first_node("ImageBehaviour"); pComp; pComp = pComp->next_sibling("ImageBehaviour"))
	{
		const auto id{ Parse<unsigned int>(pComp, "ID") };
		const auto isActive{ Parse<bool>(pComp, "Active") };
		
		const std::string imageLocation{ ParseString(pComp, "ImageLocation") };
		TextureData texData;
		LoadTexData(pComp, texData);

		HAlignment horAlignment;
		VAlignment verAlignment;
		LoadAlignments(pComp, horAlignment, verAlignment);

		auto pImageComp = new Image(id, isActive, ProtoContent.GetTexture(imageLocation), texData);
		pCurr->AddComponent(pImageComp);
		pImageComp->SetAlignment(horAlignment, verAlignment);
		pImageComp->SetTextureData(texData);
	}
}

void ProtoParser::XML::Helper::LoadTextComponents(xml_node<>* pComponents, GameObject* pCurr)
{
	for (xml_node<>* pComp = pComponents->first_node("TextBehaviour"); pComp; pComp = pComp->next_sibling("TextBehaviour"))
	{
		const auto id{ Parse<unsigned int>(pComp, "ID") };
		const auto isActive{ Parse<bool>(pComp, "Active") };
		
		const auto fontLocation{ ParseString(pComp, "FontLocation") };
		const auto fontSize{ Parse<int>(pComp, "FontSize") };

		TextureData texData;
		LoadTexData(pComp, texData);

		const auto text{ ParseString(pComp, "Text") };

		HAlignment horAlignment;
		VAlignment verAlignment;
		LoadAlignments(pComp, horAlignment, verAlignment);

		const auto pFont = !fontLocation.empty() ? ProtoContent.GetFont(fontLocation, fontSize) : nullptr;
		auto pTextComp = new Text(id, isActive, text, pFont, texData);
		pCurr->AddComponent(pTextComp);
		pTextComp->SetAlignment(horAlignment, verAlignment);
		pTextComp->SetTextureData(texData);
	}
}

void ProtoParser::XML::Helper::LoadFPSComponents(xml_node<>* pComponents, GameObject* pCurr)
{
	for (xml_node<>* pComp = pComponents->first_node("FPSBehaviour"); pComp; pComp = pComp->next_sibling("FPSBehaviour"))
	{
		const auto id{ Parse<unsigned int>(pComp, "ID") };
		const auto isActive{ Parse<bool>(pComp, "Active") };
		
		const std::string fontLocation{ ParseString(pComp, "FontLocation") };
		const int fontSize{ Parse<int>(pComp, "FontSize") };

		TextureData texData;
		LoadTexData(pComp, texData);

		HAlignment horAlignment;
		VAlignment verAlignment;
		LoadAlignments(pComp, horAlignment, verAlignment);

		const auto pFont = !fontLocation.empty() ? ProtoContent.GetFont(fontLocation, fontSize) : nullptr;
		auto pFPSComp = new FPSText(id, isActive, pFont, texData);
		pCurr->AddComponent(pFPSComp);
		pFPSComp->SetAlignment(horAlignment, verAlignment);
		pFPSComp->SetTextureData(texData);
	}
}

void ProtoParser::XML::Helper::LoadCameraComponent(xml_node<>* pComponents, GameObject* pCurr)
{
	for (xml_node<>* pComp = pComponents->first_node("CameraBehaviour"); pComp; pComp = pComp->next_sibling("CameraBehaviour"))
	{
		const auto id{ Parse<unsigned int>(pComp, "ID") };

		glm::vec2 position;
		position.x = Parse<float>(pComp, "PositionX");
		position.y = Parse<float>(pComp, "PositionY");

		const auto active{ Parse<bool>(pComp, "CamActive") };

		pCurr->AddComponent(new Camera(id, position, active));
	}
}

void ProtoParser::XML::Helper::LoadTexData(xml_node<>* pComp, TextureData& texData)
{
	texData.x =			Parse<float>(pComp, "TexDataX");
	texData.y =			Parse<float>(pComp, "TexDataY");
	texData.width =		Parse<float>(pComp, "TexDataW");
	texData.height =	Parse<float>(pComp, "TexDataH");
	texData.color.r =	Parse<Uint8>(pComp, "TexDataColorR");
	texData.color.g =	Parse<Uint8>(pComp, "TexDataColorG");
	texData.color.b =	Parse<Uint8>(pComp, "TexDataColorB");
	texData.color.a =	Parse<Uint8>(pComp, "TexDataColorA");
}

void ProtoParser::XML::Helper::LoadAlignments(xml_node<>* pComp, HAlignment& horAlignment, VAlignment& verAlignment)
{
	const std::string horAlignmentStr{ ParseString(pComp, "HorizontalAlignment") };

	if (horAlignmentStr == "Left")
		horAlignment = HAlignment::LEFT;
	else if (horAlignmentStr == "Center")
		horAlignment = HAlignment::CENTER;
	else
		horAlignment = HAlignment::RIGHT;

	const std::string verAlignmentStr{ ParseString(pComp, "VerticalAlignment") };

	if (verAlignmentStr == "Top")
		verAlignment = VAlignment::TOP;
	else if (verAlignmentStr == "Center")
		verAlignment = VAlignment::CENTER;
	else
		verAlignment = VAlignment::BOTTOM;
}

float SecondsToMicroSeconds(float seconds)
{
	return std::roundf(seconds * 1'000'000);
}

float MicroSecondsToSeconds(float microSeconds)
{
	return microSeconds / 1'000'000;
}

std::string ProtoConvert::ToString(const std::wstring& wstring)
{
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;
	return converter.to_bytes(wstring);
}

std::wstring ProtoConvert::ToWString(const std::string& string)
{
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;
	return converter.from_bytes(string);
}

b2Vec2 ProtoConvert::ToBox2DVec(const glm::vec2& vec)
{
	return b2Vec2{ float(vec.x), float(vec.y) };
}

b2Vec2 ProtoConvert::ToBox2DVec(const ImVec2& vec)
{
	return b2Vec2{ float(vec.x), float(vec.y) };
}

b2Vec3 ProtoConvert::ToBox2DVec(const glm::vec3& vec)
{
	return b2Vec3{ float(vec.x), float(vec.y), float(vec.z) };
}

ImVec2 ProtoConvert::ToImGuiVec(const glm::vec2& vec)
{
	return ImVec2{ float(vec.x), float(vec.y) };
}

ImVec2 ProtoConvert::ToImGuiVec(const b2Vec2& vec)
{
	return ImVec2{ float(vec.x), float(vec.y) };
}

ImVec4 ProtoConvert::ToImGuiVec(const glm::vec4& vec)
{
	return ImVec4{ float(vec.x), float(vec.y), float(vec.z), float(vec.w) };
}

glm::vec2 ProtoConvert::ToGLMVec(const b2Vec2& vec)
{
	return glm::vec2{ float(vec.x), float(vec.y) };
}

glm::vec2 ProtoConvert::ToGLMVec(const ImVec2& vec)
{
	return glm::vec2{ float(vec.x), float(vec.y) };
}

glm::vec3 ProtoConvert::ToGLMVec(const b2Vec3& vec)
{
	return glm::vec3{ float(vec.x), float(vec.y), float(vec.z) };
}

glm::vec4 ProtoConvert::ToGLMVec(const ImVec4& vec)
{
	return glm::vec4{ float(vec.x), float(vec.y), float(vec.z), float(vec.w) };
}

float ProtoConvert::ToDegrees(float radians)
{
	return glm::degrees(radians);
}

float ProtoConvert::ToRadians(float degrees)
{
	return glm::radians(degrees);
}

Uint32 ProtoConvert::ColorToUint(int R, int G, int B)
{
	return static_cast<Uint32>((R << 16) + (G << 8) + (B << 0));
}

SDL_Color ProtoConvert::UintToColor(Uint32 color)
{
	SDL_Color tempcol;
	tempcol.a = 255;
	tempcol.r = (color >> 16) & 0xFF;
	tempcol.g = (color >> 8) & 0xFF;
	tempcol.b = color & 0xFF;
	return tempcol;
}
