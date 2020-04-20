#include "ProtoEnginePCH.h"
#include "EditorGame.h"

void EditorGame::InitializeScene()
{
	ProtoTime.TimeScale = 0;
	ProtoScenes.AddGameScene(new Scene("\\Temp\\temp.protoscene"));

	ProtoEditor.SetSceneName("Temp");
	ProtoEditor.SetSaveFileName("temp");
	ProtoEditor.SetSaveFilePath("\\Temp");

	ProtoScenes.SetActiveGameScene(L"Temp");
}