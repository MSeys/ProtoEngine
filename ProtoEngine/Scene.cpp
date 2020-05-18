#include "ProtoEnginePCH.h"
#include "Scene.h"

#include <fstream>

#include "GameObject.h"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
#include <utility>
namespace fs = std::experimental::filesystem;

Scene::Scene(const std::string& filePath)
	: m_pChildren(std::vector<GameObject*>())
	, m_IsInitialized(false)
{
	m_FilePath = filePath;
	
	m_FileFolderStructure = m_FilePath.substr(0, m_FilePath.rfind('\\'));
	m_FileName = m_FilePath.substr(m_FileFolderStructure.size() + 1);
	m_FileName = m_FileName.substr(0, m_FileName.rfind('.'));

	/* Read Scene Name for Editor and other purposes */ {
		using namespace rapidxml;

		xml_document<> doc;
		const std::string path{ ProtoContent.GetDataPath() + m_FilePath };
		std::ifstream file(path);

		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string content(buffer.str());

		doc.parse<0>(&content[0]);

		xml_node<>* scene = doc.first_node("Scene");
		m_SceneName = ProtoConvert::ToWString(ProtoParser::XML::ParseString(scene, "Name"));
	}
}

Scene::Scene(std::wstring sceneName, std::string folderStructure, std::string fileName)
	: m_IsInitialized(false)
	, m_SceneName(std::move(sceneName).c_str()) // NOLINT(readability-redundant-string-cstr)
	, m_FileFolderStructure(std::move(folderStructure).c_str()) // NOLINT(readability-redundant-string-cstr)
	, m_FileName(std::move(fileName).c_str()) // NOLINT(readability-redundant-string-cstr)
{
	m_FilePath = m_FileFolderStructure.c_str() + std::string("\\") + m_FileName.c_str() + ".protoscene"; // NOLINT(readability-redundant-string-cstr)
}

Scene::~Scene()
{
	Reset();
}

void Scene::Save()
{
	using namespace rapidxml;

	xml_document<> doc;
	/* Basic Node Declaration - Version + Encoding */ {
		xml_node<>* decl = doc.allocate_node(node_declaration);
		decl->append_attribute(doc.allocate_attribute("version", "1.0"));
		decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
		doc.append_node(decl);
	}

	xml_node<>* scene = doc.allocate_node(node_element, "Scene");
	scene->append_attribute(doc.allocate_attribute("version", "1.0"));

	const std::string convertedSceneName{ ProtoConvert::ToString(m_SceneName.c_str()) };  // NOLINT(readability-redundant-string-cstr)
	scene->append_attribute(doc.allocate_attribute("Name", convertedSceneName.c_str()));
	scene->append_attribute(doc.allocate_attribute("type", "protoscene"));
	
	for(auto pGameObject : m_pChildren)
	{
		pGameObject->Save(doc, scene);
	}

	doc.append_node(scene);

	const std::string path{ ProtoContent.GetDataPath() + m_FileFolderStructure.c_str() + "\\" + m_FileName.c_str() + ".protoscene" };  // NOLINT(readability-redundant-string-cstr)
																														// c_str() is necessary as strings have reserved space (filled with '\0')
	fs::create_directory(ProtoContent.GetDataPath() + m_FileFolderStructure.c_str());  // NOLINT(readability-redundant-string-cstr)
	std::ofstream fileStored(path);
	fileStored << doc;
	fileStored.close();
	doc.clear();
}

void Scene::Reset()
{
	ProtoPhysics.ClearWorld();
	
	for (auto pChild : m_pChildren)
		SafeDelete(pChild);

	m_pChildren.clear();
}

void Scene::Load()
{
	LoadAs(m_FilePath);
}

void Scene::LoadAs(const std::string& filePath)
{
	using namespace rapidxml;

	xml_document<> doc;
	const std::string path{ ProtoContent.GetDataPath() + filePath };
	std::ifstream file(path);

	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string content(buffer.str());

	doc.parse<0>(&content[0]);

	xml_node<>* scene = doc.first_node("Scene");

	for (xml_node<>* gameObjectNode = scene->first_node("GameObject"); gameObjectNode; gameObjectNode = gameObjectNode->next_sibling())
	{
		const std::string goName{ ProtoParser::XML::ParseString(gameObjectNode, "Name") };
		const GameObjectID id{ ProtoParser::XML::Parse<unsigned int>(gameObjectNode, "ID") };
		const bool goActive{ ProtoParser::XML::Parse<bool>(gameObjectNode, "Active") };

		auto pNew = new GameObject(id, goName, goActive);
		AddChild(pNew);

		pNew->Load(gameObjectNode);
	}
}

#pragma region Add / Remove Child
void Scene::AddChild(GameObject* obj)
{
#if _DEBUG
	if (obj->m_pParentScene)
	{
		if (obj->m_pParentScene == this)
			std::cout << "GameScene::AddChild > GameObject is already attached to this GameScene" << std::endl;
		else
			std::cout << "GameScene::AddChild > GameObject is already attached to another GameScene. Detach it from it's current scene before attaching it to another one." << std::endl;

		return;
	}

	if (obj->m_pParentObject)
	{
		std::cout << "GameScene::AddChild > GameObject is currently attached to a GameObject. Detach it from it's current parent before attaching it to another one." << std::endl;
		return;
	}
#endif

	obj->m_pParentScene = this;
	obj->Start();
	m_pChildren.push_back(obj);
}

void Scene::RemoveChild(GameObject* obj, bool deleteObject)
{
	const auto it = find(m_pChildren.begin(), m_pChildren.end(), obj);

#if _DEBUG
	if (it == m_pChildren.end())
	{
		std::cout << "GameScene::RemoveChild > GameObject to remove is not attached to this GameScene!" << std::endl;
		return;
	}
#endif

	m_pChildren.erase(it);
	if (deleteObject)
		SafeDelete(obj);
	else
		obj->m_pParentScene = nullptr;
}
#pragma endregion Add / Remove Child

void Scene::SwapUpChild(GameObject* obj)
{
	const auto it{ std::find(m_pChildren.begin(), m_pChildren.end(), obj) };
	if (it != m_pChildren.begin())
		std::swap(*it, *(it - 1));
}

void Scene::SwapDownChild(GameObject* obj)
{
	const auto it{ std::find(m_pChildren.begin(), m_pChildren.end(), obj) };
	if (it != m_pChildren.end() - 1)
		std::swap(*it, *(it + 1));
}

#pragma region Root Functions
void Scene::DrawHierarchy()
{
	for (auto pChild : m_pChildren)
	{
		ImGui::PushID(pChild);
		pChild->DrawHierarchy();
		ImGui::PopID();
	}
}

GameObject* Scene::FindGameObjectWithID(GameObjectID id) const
{
	GameObject* pFoundHere = *std::find_if(m_pChildren.cbegin(), m_pChildren.cend(), [id](GameObject* pObject) { return unsigned(pObject->GetID()) == unsigned(id); });
	if (pFoundHere)
		return pFoundHere;
	
	for (GameObject* pGameObject : m_pChildren)
	{
		GameObject* pFoundInChildren = pGameObject->FindGameObjectWithIDinChildren(id);
		if (pFoundInChildren)
			return pFoundInChildren;
	}

	return nullptr;
}

void Scene::SetActiveCamera(Camera* pCamera)
{
	if (m_pActiveCamera)
		m_pActiveCamera->Deactivate();

	m_pActiveCamera = pCamera;
}

glm::vec2 Scene::GetActiveCamera() const
{
	if (m_pActiveCamera)
		return m_pActiveCamera->GetPosition();

	return { 0, 0 };
}

void Scene::Start()
{
	for (auto pChild : m_pChildren)
		pChild->Start();
}

void Scene::Awake()
{
	for (auto pChild : m_pChildren)
		pChild->Awake();
}

void Scene::Update()
{
	for (auto pChild : m_pChildren)
		pChild->Update();
}

void Scene::FixedUpdate()
{
	for (auto pChild : m_pChildren)
		pChild->FixedUpdate();
}

void Scene::Draw()
{
	for (auto pChild : m_pChildren)
		pChild->Draw();
}
#pragma endregion Root Functions