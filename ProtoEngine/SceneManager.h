#pragma once
#include "Singleton.h"

class BaseScene;

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

		void AddGameScene(BaseScene* pScene);
		void RemoveGameScene(BaseScene* pScene);
		void SetActiveGameScene(const std::wstring& sceneName);
		void NextScene();
		void PreviousScene();
		BaseScene* GetActiveScene() const { return m_ActiveScene; }

		void Update();
		void Draw() const;
		
	private:
		friend class Singleton<SceneManager>;

		SceneManager();

		void Initialize();

		std::vector<BaseScene*> m_pScenes;
		bool m_IsInitialized;
		BaseScene* m_ActiveScene, * m_NewActiveScene;
	};
}
