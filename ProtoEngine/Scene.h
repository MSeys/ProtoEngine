#pragma once
#include "CameraComponent.h"
class GameObject;

namespace Proto {
	class SceneManager;
	class Editor;
}

class Scene
{
public:
	Scene(const std::string& filePath = "");
	virtual ~Scene();
	
	Scene(const Scene& other) = delete;
	Scene(Scene&& other) noexcept = delete;
	Scene& operator=(const Scene& other) = delete;
	Scene& operator=(Scene&& other) noexcept = delete;

	void AddChild(GameObject* obj);
	void RemoveChild(GameObject* obj, bool deleteObject = true);

	bool HasActiveCamera() const { return m_pActiveCamera != nullptr; }
	glm::vec2 GetActiveCamera() const;
	void SetActiveCamera(CameraComponent* pCamera);
	
protected:
	void DrawHierarchy();

	void Save(const std::string& filePath, const std::string& fileName);
	void Load(const std::string& filePath, std::string* pFolderPath = nullptr, std::string* pFileName = nullptr);

	void SwapUpChild(GameObject* obj);
	void SwapDownChild(GameObject* obj);

	std::vector<GameObject*>& GetChildren() { return m_pChildren; }
	std::wstring GetSceneName() const { return m_SceneName; }
	void SetSceneName(const std::wstring& sceneName) { m_SceneName = sceneName.c_str(); }  // NOLINT(readability-redundant-string-cstr)
	
	std::string GetFilePath() const { return m_FilePath; }
	void SetFilePath(const std::string& filePath) { m_FilePath = filePath; }
	std::string GetFileName() const { return m_FileName; }
	void SetFileName(const std::string& fileName) { m_FileName = fileName; }

	GameObjectID RequestNewID() { return ++m_CurrentID; }
	GameObjectID GetCurrentID() const { return m_CurrentID; }
	void SetCurrentID(const GameObjectID& id) { m_CurrentID = id; }

	GameObject* FindGameObjectWithID(GameObjectID id) const;
	
private:
	friend class Proto::SceneManager;
	friend class Proto::Editor;
	friend class GameObject;
	friend class CameraComponent;

	void Start();
	void Awake();
	void Update();
	void FixedUpdate();
	void Draw();
	
	std::vector<GameObject*> m_pChildren;
	bool m_IsInitialized;
	std::wstring m_SceneName;

	std::string m_FilePath;
	std::string m_FileName;

	GameObjectID m_CurrentID{ 0 };
	CameraComponent* m_pActiveCamera{};
};