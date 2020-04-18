#pragma once
#include "Singleton.h"

class Scene;

namespace Proto
{
	class SceneManager final : public Singleton<SceneManager>
	{
	public:
		~SceneManager();
		SceneManager(const SceneManager& other) = delete;
		SceneManager(SceneManager&& other) noexcept = delete;
		SceneManager& operator=(const SceneManager& other) = delete;
		SceneManager& operator=(SceneManager&& other) noexcept = delete;

		void AddGameScene(Scene* pScene);
		void RemoveGameScene(Scene* pScene);
		void SetActiveGameScene(const std::wstring& sceneName);
		void NextScene();
		void PreviousScene();
		Scene* GetActiveScene() const { return m_ActiveScene; }

		void Update();
		void FixedUpdate();
		void Draw() const;

		void DrawHierarchy() const;
		
	private:
		friend class Singleton<SceneManager>;

		SceneManager();

		void Initialize();

		std::vector<Scene*> m_pScenes;
		bool m_IsInitialized;
		Scene* m_ActiveScene, * m_NewActiveScene;
	};
}
