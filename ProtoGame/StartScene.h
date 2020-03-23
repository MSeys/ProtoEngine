#pragma once
#include "BaseScene.h"

class StartScene : public BaseScene
{
public:
	StartScene(const std::wstring& sceneName);
	~StartScene() override = default;
	
protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void SceneActivated() override;
	void SceneDeactivated() override;
};

