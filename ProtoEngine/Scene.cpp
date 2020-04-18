#include "ProtoEnginePCH.h"
#include "Scene.h"

#include <fstream>

#include "GameObject.h"
#include "Utils.h"

Scene::Scene(std::wstring sceneName, const std::string& filePath)
	: m_pChildren(std::vector<GameObject*>())
	, m_IsInitialized(false)
	, m_SceneName(std::move(sceneName))
{
	if (!filePath.empty())
		Load(filePath);
}

Scene::~Scene()
{
	for (auto pChild : m_pChildren)
	{
		SafeDelete(pChild);
	}
}

void Scene::Save(const std::string& filePath, const std::string& fileName)
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
	scene->append_attribute(doc.allocate_attribute("type", "protoscene"));
	
	for(auto pGameObject : m_pChildren)
	{
		pGameObject->Save(doc, scene);
	}

	doc.append_node(scene);

	const std::string path{ ProtoContent.GetDataPath() + filePath.c_str() + "\\" + fileName.c_str() + ".protoscene" };  // NOLINT(readability-redundant-string-cstr)
																														// c_str() is necessary as strings have reserved space (filled with '\0')
	std::ofstream fileStored(path);
	fileStored << doc;
	fileStored.close();
	doc.clear();
}

void Scene::Load(const std::string& filePath, std::string* pFolderPath, std::string* pFileName)
{
	if(pFolderPath && pFileName)
	{
		*pFolderPath = filePath.substr(0, filePath.rfind('\\'));
		*pFileName = filePath.substr(pFolderPath->size() + 1);
		*pFileName = pFileName->substr(0, pFileName->rfind('.'));
	}
	
	using namespace rapidxml;

	xml_document<> doc;
	const std::string path{ ProtoContent.GetDataPath() + filePath };
	std::ifstream file(path);

	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string content(buffer.str());

	doc.parse<0>(&content[0]);

	xml_node<>* scene = doc.first_node("Scene");
	for(xml_node<>* gameObjectNode = scene->first_node("GameObject"); gameObjectNode; gameObjectNode = gameObjectNode->next_sibling())
	{
		const std::string goName{ gameObjectNode->first_attribute("Name")->value() };
		const bool goActive{ std::string(gameObjectNode->first_attribute("Active")->value()) == "true" }; 
		
		auto pNew = new GameObject(goName, goActive);
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
	{
		std::swap(*it, *(it - 1));
	}
}

void Scene::SwapDownChild(GameObject* obj)
{
	const auto it{ std::find(m_pChildren.begin(), m_pChildren.end(), obj) };
	if (it != m_pChildren.end() - 1)
	{
		std::swap(*it, *(it + 1));
	}
}

#pragma region Root Functions
void Scene::DrawHierarchy()
{
	for (auto pChild : m_pChildren)
	{
		pChild->DrawHierarchy();
	}
}

void Scene::Start()
{
	if (m_IsInitialized)
		return;

	for (auto pChild : m_pChildren)
	{
		pChild->Start();
	}

	m_IsInitialized = true;
}

void Scene::Awake()
{
	for (auto pChild : m_pChildren)
	{
		pChild->Awake();
	}
}

void Scene::Update()
{
	for (auto pChild : m_pChildren)
	{
		pChild->Update();
	}
}

void Scene::FixedUpdate()
{
	for (auto pChild : m_pChildren)
	{
		pChild->FixedUpdate();
	}
}

void Scene::Draw()
{
	for (auto pChild : m_pChildren)
	{
		pChild->Draw();
	}
}
#pragma endregion Root Functions