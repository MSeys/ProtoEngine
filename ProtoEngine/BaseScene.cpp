#include "ProtoEnginePCH.h"
#include "BaseScene.h"
#include "GameObject.h"
#include "Utils.h"


BaseScene::BaseScene(std::wstring sceneName)
	: m_pChildren(std::vector<GameObject*>())
	, m_IsInitialized(false)
	, m_SceneName(std::move(sceneName))
{
}

BaseScene::~BaseScene()
{
	for (auto pChild : m_pChildren)
	{
		SafeDelete(pChild);
	}
}

#pragma region Add / Remove Child
void BaseScene::AddChild(GameObject* obj)
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
	obj->RootInitialize();
	m_pChildren.push_back(obj);
}

void BaseScene::RemoveChild(GameObject* obj, bool deleteObject)
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
	{
		delete obj;
		obj = nullptr;
	}
	else
		obj->m_pParentScene = nullptr;
}
#pragma endregion Add / Remove Child

void BaseScene::SwapUpChild(GameObject* obj)
{
	const auto it{ std::find(m_pChildren.begin(), m_pChildren.end(), obj) };
	if (it != m_pChildren.begin())
	{
		std::swap(*it, *(it - 1));
	}
}

void BaseScene::SwapDownChild(GameObject* obj)
{
	const auto it{ std::find(m_pChildren.begin(), m_pChildren.end(), obj) };
	if (it != m_pChildren.end() - 1)
	{
		std::swap(*it, *(it + 1));
	}
}

#pragma region Root Functions
void BaseScene::DrawHierarchy()
{
	for (auto pChild : m_pChildren)
	{
		pChild->DrawHierarchy();
	}
}

void BaseScene::RootInitialize()
{
	if (m_IsInitialized)
		return;

	//User-Scene Initialize
	InitializeCommands();
	InitializeControls();
	InitializeScene();

	//Root-Scene Initialize
	for (auto pChild : m_pChildren)
	{
		pChild->RootInitialize();
	}

	m_IsInitialized = true;
}

void BaseScene::RootUpdate()
{
	//User-Scene Update
	Update();

	//Root-Scene Update
	for (auto pChild : m_pChildren)
	{
		pChild->RootUpdate();
	}
}

void BaseScene::RootFixedUpdate()
{
	//User-Scene Update
	FixedUpdate();

	//Root-Scene Update
	for (auto pChild : m_pChildren)
	{
		pChild->RootFixedUpdate();
	}
}

void BaseScene::RootDraw()
{
	//User-Scene Draw
	Draw();

	//Object-Scene Draw
	for (auto pChild : m_pChildren)
	{
		pChild->RootDraw();
	}
}

void BaseScene::RootSceneActivated()
{
	SceneActivated();
}

void BaseScene::RootSceneDeactivated()
{
	SceneDeactivated();
}
#pragma endregion Root Functions