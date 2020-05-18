#include "ProtoEnginePCH.h"
#include "EditorGame.h"

void EditorGame::InitializeScene()
{
	ProtoTime.TimeScale = 0;
	ProtoScenes.Add(new Scene("\\Temp\\temp.protoscene"));

	ProtoEditor.SetSceneName("Temp");
	ProtoEditor.SetSaveFileName("temp");
	ProtoEditor.SetSaveFileFolderStructure("\\Temp");

	ProtoScenes.SetCurrentScene(L"Temp");
}