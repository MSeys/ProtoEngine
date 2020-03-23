#pragma once

#include <stdio.h>
#include <iostream> // std::cout
#include <sstream> // stringstream
#include <memory> // smart pointers
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

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

#include "Utils.h"
#include "Components.h"
#include "Structs.h"