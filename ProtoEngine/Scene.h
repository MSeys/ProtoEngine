#pragma once
class GameObject;

namespace Proto {
	class SceneManager;
}

class Scene
{
public:
	Scene(std::wstring sceneName, const std::string& filePath = "");
	virtual ~Scene();
	
	Scene(const Scene& other) = delete;
	Scene(Scene&& other) noexcept = delete;
	Scene& operator=(const Scene& other) = delete;
	Scene& operator=(Scene&& other) noexcept = delete;

	void Save(const std::string& filePath, const std::string& fileName);
	void Load(const std::string& filePath, std::string* pFolderPath = nullptr, std::string* pFileName = nullptr);
	
	void AddChild(GameObject* obj);
	void RemoveChild(GameObject* obj, bool deleteObject = true);

	void SwapUpChild(GameObject* obj);
	void SwapDownChild(GameObject* obj);

	std::vector<GameObject*>& GetChildren() { return m_pChildren; }
	
protected:
	void DrawHierarchy();

private:
	friend class Proto::SceneManager;

	void Start();
	void Awake();
	void Update();
	void FixedUpdate();
	void Draw();
	
	std::vector<GameObject*> m_pChildren;
	bool m_IsInitialized;
	std::wstring m_SceneName;
};
