#pragma once
#include "BaseScene.h"

class StartScene : public BaseScene
{
public:
	StartScene(const std::wstring& sceneName);
	~StartScene() override = default;
	
protected:
	void InitializeCommands() override;
	void InitializeControls() override;
	void InitializeScene() override;
	void Update() override;
	void Draw() override;
	void SceneActivated() override;
	void SceneDeactivated() override;

	void FixedUpdate() override;
private:
	std::string m_JumpDataExample{ "Boing!" };
	AudioServices m_AudioServices{};
};