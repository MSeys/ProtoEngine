// ReSharper disable CppClangTidyCppcoreguidelinesMacroUsage
#pragma once

#include <cstdio>
#include <iostream> // std::cout
#include <sstream> // stringstream
#include <memory> // smart pointers
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define STRINGIFY(x) #x
#define CompStringify(x, y) bool((x) == #y)

enum class StickState;
typedef WORD XINPUT_Keycode;
typedef StickState MouseState;

// Define Singletons

#include "PhysicsManager.h"
#define ProtoPhysics Proto::PhysicsManager::GetInstance()

#include "SettingsManager.h"
#define ProtoSettings Proto::SettingsManager::GetInstance()

#include "Logger.h"
#define ProtoLogger Proto::Logger::GetInstance()

#include "Renderer.h"
#define ProtoRenderer Proto::Renderer::GetInstance() 

#include "InputManager.h"
#define ProtoInput Proto::InputManager::GetInstance()

#include "ContentManager.h"
#define ProtoContent Proto::ContentManager::GetInstance()

#include "SceneManager.h"
#define ProtoScenes Proto::SceneManager::GetInstance()

#include "CommandManager.h"
#define ProtoCommands Proto::CommandManager::GetInstance()

#include "TimeManager.h"
#define ProtoTime Proto::TimeManager::GetInstance()

#include "Audio.h"
#define ProtoAudio AudioServiceLocator

#include "Messenger.h"
#define ProtoMessenger Proto::Messenger::GetInstance()

#include "AchievementManager.h"
#define ProtoAchievements(x) Proto::AchievementManager<x>::GetInstance()



#include "Editor.h"
#define ProtoEditor Proto::Editor::GetInstance()


#include "Texture2D.h"
#include "Font.h"

#include "ProtoUtils.h"
#include "EngineBehaviours.h"
#include "Structs.h"

#include "Buttons.h"
#include "Joysticks.h"
#include "Triggers.h"

#pragma warning(push)
#pragma warning(disable : 4201)
#include "glm/glm.hpp"
#pragma warning(pop)

#include "ImGui/imgui.h"
#include "ImGui/imgui_sdl.h"
#include "ProtoGui.h"

#include "portable-file-dialogs.h"
#include "RapidXML/rapidxml.hpp"
#include "RapidXML/rapidxml_iterators.hpp"
#include "RapidXML/rapidxml_print.hpp"
#include "RapidXML/rapidxml_utils.hpp"

#include "Box2D.h"