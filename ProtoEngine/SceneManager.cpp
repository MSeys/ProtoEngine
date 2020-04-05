#include "ProtoEnginePCH.h"
#include "SceneManager.h"

#include <SDL.h>

#include "BaseScene.h"
#include "Utils.h"

Proto::SceneManager::SceneManager()
	: m_pScenes(std::vector<BaseScene*>())
	, m_IsInitialized(false)
	, m_ActiveScene(nullptr)
	, m_NewActiveScene(nullptr)
{
	Initialize();
}

Proto::SceneManager::~SceneManager()
{
	for (BaseScene* scene : m_pScenes)
	{
		SafeDelete(scene);
	}
}

void Proto::SceneManager::Initialize()
{
	if (m_IsInitialized)
		return;

	for (BaseScene* scene : m_pScenes)
	{
		scene->RootInitialize();
	}

	m_IsInitialized = true;
}

void Proto::SceneManager::Update()
{	
	if (m_NewActiveScene != nullptr)
	{
		//Deactivate the current active scene
		if (m_ActiveScene != nullptr)
			m_ActiveScene->RootSceneDeactivated();

		//Set New Scene
		m_ActiveScene = m_NewActiveScene;
		m_NewActiveScene = nullptr;

		//Active the new scene and reset SceneTimer
		m_ActiveScene->RootSceneActivated();
	}

	if (m_ActiveScene != nullptr)
	{
		m_ActiveScene->RootUpdate();
	}
}

void Proto::SceneManager::Draw() const
{
	if (m_ActiveScene != nullptr)
		m_ActiveScene->RootDraw();
}

void Proto::SceneManager::AddGameScene(BaseScene* pScene)
{
	const auto it = find(m_pScenes.begin(), m_pScenes.end(), pScene);

	if (it == m_pScenes.end())
	{
		m_pScenes.push_back(pScene);

		if (m_IsInitialized)
			pScene->RootInitialize();

		if (m_ActiveScene == nullptr && m_NewActiveScene == nullptr)
			m_NewActiveScene = pScene;
	}
}

void Proto::SceneManager::RemoveGameScene(BaseScene* pScene)
{
	const auto it = find(m_pScenes.begin(), m_pScenes.end(), pScene);

	if (it != m_pScenes.end())
	{
		m_pScenes.erase(it);
	}
}

void Proto::SceneManager::SetActiveGameScene(const std::wstring& sceneName)
{
	const auto it = find_if(m_pScenes.begin(), m_pScenes.end(), [sceneName](BaseScene* scene)
		{
			return wcscmp(scene->m_SceneName.c_str(), sceneName.c_str()) == 0;
		});

	if (it != m_pScenes.end())
	{
		m_NewActiveScene = *it;
	}
}

void Proto::SceneManager::NextScene()
{
	for (unsigned int i = 0; i < m_pScenes.size(); ++i)
	{
		if (m_pScenes[i] == m_ActiveScene)
		{
			const auto nextScene = ++i % m_pScenes.size();
			m_NewActiveScene = m_pScenes[nextScene];
			break;
		}
	}
}

void Proto::SceneManager::PreviousScene()
{
	for (unsigned int i = 0; i < m_pScenes.size(); ++i)
	{
		if (m_pScenes[i] == m_ActiveScene)
		{
			if (i == 0) i = unsigned(m_pScenes.size());
			--i;
			m_NewActiveScene = m_pScenes[i];
			break;
		}
	}
}
