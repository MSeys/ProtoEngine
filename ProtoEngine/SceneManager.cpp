#include "ProtoEnginePCH.h"
#include "SceneManager.h"

#include "Scene.h"

Proto::SceneManager::SceneManager()
	: m_pScenes(std::vector<Scene*>())
	, m_IsInitialized(false)
	, m_pCurrentScene(nullptr)
{
}

Proto::SceneManager::~SceneManager()
{
	for (Scene* scene : m_pScenes)
		SafeDelete(scene);
}

void Proto::SceneManager::Update() const
{	
	if (m_pCurrentScene != nullptr)
		m_pCurrentScene->Update();
}

void Proto::SceneManager::UpdateUnscaled() const
{
	if (m_pCurrentScene != nullptr)
		m_pCurrentScene->UpdateUnscaled();
}

void Proto::SceneManager::FixedUpdate() const
{
	if (m_pCurrentScene != nullptr)
		m_pCurrentScene->FixedUpdate();
}

void Proto::SceneManager::Draw() const
{
	if (m_pCurrentScene != nullptr)
		m_pCurrentScene->Draw();
}

void Proto::SceneManager::DrawHierarchy() const
{
	if (m_pCurrentScene != nullptr)
		m_pCurrentScene->DrawHierarchy();
}

void Proto::SceneManager::Add(Scene* pScene)
{
	const auto it = find(m_pScenes.begin(), m_pScenes.end(), pScene);

	if (it == m_pScenes.end())
		m_pScenes.push_back(pScene);
}

void Proto::SceneManager::Load(const std::wstring& sceneName)
{
	const auto it = find_if(m_pScenes.begin(), m_pScenes.end(), [sceneName](Scene* scene)
		{
			return wcscmp(scene->m_SceneName.c_str(), sceneName.c_str()) == 0;
		});

	if (it != m_pScenes.end())
	{
		// Reset editor selected to avoid crashes
		ProtoEditor.SetCurrentSelected(nullptr);
		
		m_pCurrentScene = *it;
		m_pCurrentScene->Reset(); // Reset scene

		ProtoCommands.Reset();
		ProtoInput.Reset();
		
		m_pCurrentScene->Load(); // Reload it from file
		
		m_pCurrentScene->Start(); // Call Start
		m_pCurrentScene->Awake(); // Call Awake
	}
}

void Proto::SceneManager::SetCurrentScene(const std::wstring& sceneName)
{
	const auto it = find_if(m_pScenes.begin(), m_pScenes.end(), [sceneName](Scene* scene)
		{
			return wcscmp(scene->m_SceneName.c_str(), sceneName.c_str()) == 0;
		});

	if (it != m_pScenes.end())
	{
		m_pCurrentScene = *it;
	}
}

void Proto::SceneManager::Begin()
{
	if (m_IsInitialized)
		return;

	if(m_pCurrentScene != nullptr)
	{
		m_pCurrentScene->Load();
		
		m_pCurrentScene->Start();
		m_pCurrentScene->Awake();
	}

	m_IsInitialized = true;
}