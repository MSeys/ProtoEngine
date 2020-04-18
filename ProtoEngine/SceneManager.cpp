#include "ProtoEnginePCH.h"
#include "SceneManager.h"

#include <SDL.h>

#include "Scene.h"
#include "Utils.h"

Proto::SceneManager::SceneManager()
	: m_pScenes(std::vector<Scene*>())
	, m_IsInitialized(false)
	, m_ActiveScene(nullptr)
	, m_NewActiveScene(nullptr)
{
	Initialize();
}

Proto::SceneManager::~SceneManager()
{
	for (Scene* scene : m_pScenes)
	{
		SafeDelete(scene);
	}
}

void Proto::SceneManager::Initialize()
{
	if (m_IsInitialized)
		return;

	for (Scene* scene : m_pScenes)
	{
		scene->Start();
		scene->Awake();
	}

	m_IsInitialized = true;
}

void Proto::SceneManager::Update()
{	
	if (m_NewActiveScene != nullptr)
	{
		//Set New Scene
		m_ActiveScene = m_NewActiveScene;
		m_NewActiveScene = nullptr;
	}

	if (m_ActiveScene != nullptr)
	{
		m_ActiveScene->Update();
	}
}

void Proto::SceneManager::FixedUpdate()
{
	if (m_ActiveScene != nullptr)
	{
		m_ActiveScene->FixedUpdate();
	}
}

void Proto::SceneManager::Draw() const
{
	if (m_ActiveScene != nullptr)
		m_ActiveScene->Draw();
}

void Proto::SceneManager::DrawHierarchy() const
{
	if (m_ActiveScene != nullptr)
		m_ActiveScene->DrawHierarchy();
}

void Proto::SceneManager::AddGameScene(Scene* pScene)
{
	const auto it = find(m_pScenes.begin(), m_pScenes.end(), pScene);

	if (it == m_pScenes.end())
	{
		m_pScenes.push_back(pScene);

		if (m_IsInitialized)
			pScene->Start();

		if (m_ActiveScene == nullptr && m_NewActiveScene == nullptr)
			m_NewActiveScene = pScene;
	}
}

void Proto::SceneManager::RemoveGameScene(Scene* pScene)
{
	const auto it = find(m_pScenes.begin(), m_pScenes.end(), pScene);

	if (it != m_pScenes.end())
	{
		m_pScenes.erase(it);
	}
}

void Proto::SceneManager::SetActiveGameScene(const std::wstring& sceneName)
{
	const auto it = find_if(m_pScenes.begin(), m_pScenes.end(), [sceneName](Scene* scene)
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
