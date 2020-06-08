#include "ProtoEnginePCH.h"
#include "ProtoUtils.h"

using namespace rapidxml;

void ProtoSaver::XML::SaveString(const char* name, std::string& value, xml_document<>& doc, xml_node<>* pComp)
{
	pComp->append_attribute(doc.allocate_attribute(name, doc.allocate_string(value.c_str())));
}

void ProtoParser::XML::LoadComponents(xml_node<>* pComponents, GameObject* pCurr)
{
	Load<Transform>(pComponents, pCurr, false);
	Load<Camera>(pComponents, pCurr, true);

	Load<RigidBody2D>(pComponents, pCurr, false);
	Load<BoxCollider2D>(pComponents, pCurr, true);
	Load<SphereCollider2D>(pComponents, pCurr, true);
	Load<LineCollider2D>(pComponents, pCurr, true);

	Load<Sprite>(pComponents, pCurr, true);
	Load<Text>(pComponents, pCurr, true);
	Load<FPSText>(pComponents, pCurr, true);
	
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

// Found at
// shifts bits to get the color value
// https://gist.github.com/exsersewo/ac1b4ff4470a7f6b443cef34e26d72ad
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

glm::vec2 ProtoConvert::PixelsToBox2D(const glm::vec2& point)
{
	return { point.x / ProtoPhysics.GetPPM(), point.y / ProtoPhysics.GetPPM() };
}

glm::vec2 ProtoConvert::Box2DToPixels(const glm::vec2& point)
{
	return { point.x * ProtoPhysics.GetPPM(), point.y * ProtoPhysics.GetPPM() };
}

glm::vec2 ProtoConvert::RotatePoint(const glm::vec2& point, const glm::vec2& center, float angle)
{
	// Used as reference (there's other examples out there)
	// https://stackoverflow.com/questions/12161277/how-to-rotate-a-vertex-around-a-certain-point
	const float x = center.x + (point.x - center.x) * cosf(angle) - (point.y - center.y) * sinf(angle);
	const float y = center.y + (point.x - center.x) * sinf(angle) + (point.y - center.y) * cosf(angle);

	return { x, y };
}
