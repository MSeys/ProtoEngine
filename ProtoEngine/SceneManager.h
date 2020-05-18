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

		void Add(Scene* pScene);
		void Load(const std::wstring& sceneName);

		// Does not reload the Scene - Use with caution
		void SetCurrentScene(const std::wstring& sceneName);
		
		Scene* GetCurrentScene() const { return m_pCurrentScene; }
		std::vector<Scene*> GetScenes() const { return m_pScenes; }
		
		void Begin();
		void Update() const;
		void FixedUpdate() const;
		void Draw() const;

		void DrawHierarchy() const;
		
	private:
		friend class Singleton<SceneManager>;

		SceneManager();

		std::vector<Scene*> m_pScenes;
		bool m_IsInitialized;
		Scene* m_pCurrentScene;
	};
}
