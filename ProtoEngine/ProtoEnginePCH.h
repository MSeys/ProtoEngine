#pragma once

#include <stdio.h>
#include <iostream> // std::cout
#include <sstream> // stringstream
#include <memory> // smart pointers
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define COMMAND_DEFAULT "DFLT_C"  // NOLINT(cppcoreguidelines-macro-usage)
#define COMMAND_EXIT "EXIT_C"  // NOLINT(cppcoreguidelines-macro-usage)

// Define Singletons
#include "Renderer.h"
#define ProtoRenderer Proto::Renderer::GetInstance()  // NOLINT(cppcoreguidelines-macro-usage)

#include "InputManager.h"
#define ProtoInput Proto::InputManager::GetInstance() // NOLINT(cppcoreguidelines-macro-usage)

#include "ResourceManager.h"
#define ProtoResources Proto::ResourceManager::GetInstance() // NOLINT(cppcoreguidelines-macro-usage)

#include "SceneManager.h"
#define ProtoScenes Proto::SceneManager::GetInstance() // NOLINT(cppcoreguidelines-macro-usage)

#include "TimeManager.h"
#define ProtoTime Proto::TimeManager::GetInstance() // NOLINT(cppcoreguidelines-macro-usage)

#include "CommandManager.h"
#define ProtoCommands Proto::CommandManager::GetInstance() // NOLINT(cppcoreguidelines-macro-usage)

#include "Audio.h"
#define ProtoAudio ServiceLocator

#include "Utils.h"
#include "Components.h"
#include "Structs.h"
#include "Buttons.h"