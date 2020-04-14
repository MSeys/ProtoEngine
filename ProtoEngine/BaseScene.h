#pragma once
class GameObject;

namespace Proto {
	class SceneManager;
}

class BaseScene
{
public:
	BaseScene(std::wstring sceneName);
	virtual ~BaseScene();
	
	BaseScene(const BaseScene& other) = delete;
	BaseScene(BaseScene&& other) noexcept = delete;
	BaseScene& operator=(const BaseScene& other) = delete;
	BaseScene& operator=(BaseScene&& other) noexcept = delete;

	void AddChild(GameObject* obj);
	void RemoveChild(GameObject* obj, bool deleteObject = true);

	void SwapUpChild(GameObject* obj);
	void SwapDownChild(GameObject* obj);

protected:
	virtual void InitializeCommands() = 0;
	virtual void InitializeControls() = 0;
	virtual void InitializeScene() = 0;
	virtual void Update() = 0;
	virtual void FixedUpdate() = 0;
	virtual void Draw() = 0;

	void DrawHierarchy();

	virtual void SceneActivated() {}
	virtual void SceneDeactivated() {}

private:
	friend class Proto::SceneManager;

	void RootInitialize();
	void RootUpdate();
	void RootFixedUpdate();
	void RootDraw();
	void RootSceneActivated();
	void RootSceneDeactivated();
	
	std::vector<GameObject*> m_pChildren;
	bool m_IsInitialized;
	std::wstring m_SceneName;
};
