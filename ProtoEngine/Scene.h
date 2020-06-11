#pragma once

class Camera;
class GameObject;

namespace Proto {
	class SceneManager;
	class Editor;
}

class Scene
{
public:
	Scene(const std::string& filePath);
	Scene(std::wstring sceneName, std::string folderStructure, std::string fileName);
	virtual ~Scene();
	
	Scene(const Scene& other) = delete;
	Scene(Scene&& other) noexcept = delete;
	Scene& operator=(const Scene& other) = delete;
	Scene& operator=(Scene&& other) noexcept = delete;

	void AddChild(GameObject* obj);
	void RemoveChild(GameObject* obj, bool deleteObject = true);

	bool HasActiveCamera() const { return m_pActiveCamera != nullptr; }
	glm::vec2 GetActiveCamera() const;
	void SetActiveCamera(Camera* pCamera);
	
protected:
	void DrawHierarchy();
	void DrawDebug();
	
	void Save();
	void Reset();
	void Load();
	void LoadAs(const std::string& filePath);

	void SwapUpChild(GameObject* obj);
	void SwapDownChild(GameObject* obj);

	std::vector<GameObject*>& GetChildren() { return m_pChildren; }
	std::wstring GetSceneName() const { return m_SceneName; }
	void SetSceneName(const std::wstring& sceneName) { m_SceneName = sceneName.c_str(); }  // NOLINT(readability-redundant-string-cstr)
	void SetFileFolderStructure(const std::string& fileFolderStructure) { m_FileFolderStructure = fileFolderStructure.c_str(); }
	void SetFileName(const std::string& fileName) { m_FileName = fileName.c_str(); }
	
	std::string GetFilePath() const { return m_FilePath; }
	std::string GetFileFolderStructure() const { return m_FileFolderStructure; }
	std::string GetFileName() const { return m_FileName; }
	
	GameObjectID RequestNewID() { return ++m_CurrentID; }
	GameObjectID GetCurrentID() const { return m_CurrentID; }
	void SetCurrentID(const GameObjectID& id) { m_CurrentID = id; }

	GameObject* FindGameObjectWithID(GameObjectID id) const;
	
private:
	friend class Proto::SceneManager;
	friend class Proto::Editor;
	friend class GameObject;
	friend class Camera;
	friend class BaseBehaviour;

	void Start();
	void Awake();
	void Update();
	void FixedUpdate();
	void Draw();
	
	std::vector<GameObject*> m_pChildren;
	bool m_IsInitialized;
	std::wstring m_SceneName;

	std::string m_FilePath;
	std::string m_FileFolderStructure;
	std::string m_FileName;

	GameObjectID m_CurrentID{ 0 };
	Camera* m_pActiveCamera{};
};