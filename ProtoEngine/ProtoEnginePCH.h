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

// Define Singletons
#include "Renderer.h"
#define ProtoRenderer Proto::Renderer::GetInstance() 

#include "InputManager.h"
#define ProtoInput Proto::InputManager::GetInstance()

#include "ResourceManager.h"
#define ProtoResources Proto::ResourceManager::GetInstance()

#include "SceneManager.h"
#define ProtoScenes Proto::SceneManager::GetInstance()

#include "TimeManager.h"
#define ProtoTime Proto::TimeManager::GetInstance()

#include "CommandManager.h"
#define ProtoCommands Proto::CommandManager::GetInstance()

#include "Audio.h"
#define ProtoAudio ServiceLocator

#include "Utils.h"
#include "Components.h"
#include "Structs.h"
#include "Buttons.h"