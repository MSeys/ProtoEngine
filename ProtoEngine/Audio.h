#pragma once
#include <iostream>
#include <windows.h>
#include <mmsystem.h>
#include <string>

#include "Logger.h"

class Audio
{
public:
	virtual ~Audio() = default;
	virtual void PlayAudio(const std::wstring& soundID) = 0;
	virtual void StopAudio(const std::wstring& soundID) = 0;
	virtual void StopAllSounds() = 0;
};

class NullAudio final : public Audio
{
public:
	void PlayAudio(const std::wstring& soundID) override { UNREFERENCED_PARAMETER(soundID); }
	void StopAudio(const std::wstring& soundID) override { UNREFERENCED_PARAMETER(soundID); }
	void StopAllSounds() override {}
};

class LogAudio : public Audio
{
public:
	void PlayAudio(const std::wstring& soundID) override { std::wcout << L"LogAudio::PlayAudio < Data\\" << soundID << std::endl; }
	void StopAudio(const std::wstring& soundID) override { std::wcout << L"LogAudio::StopAudio < Data\\" << soundID << std::endl; }
	void StopAllSounds() override { std::cout << "LogAudio::StopAllSounds" << std::endl; }
};

class PlayerAudio : public Audio
{
public:
	void PlayAudio(const std::wstring& soundID) override
	{
		std::wstring path{ L"..\\Data\\" + soundID };
		PlaySound(path.c_str(), NULL, SND_FILENAME | SND_ASYNC);
	}
	void StopAudio(const std::wstring& soundID) override { UNREFERENCED_PARAMETER(soundID); }
	void StopAllSounds() override {}
};

class AudioServiceLocator
{
public:
	static void Init() { audioService = &defaultAudioService; }
	static Audio& GetAudio() { return *audioService; }
	static void RegisterAudioService(Audio* service)
	{
		audioService = service == nullptr ? &defaultAudioService : service;
	}

private:
	static Audio* audioService;
	static NullAudio defaultAudioService;
};

struct AudioServices
{
	LogAudio LogAudio{};
	PlayerAudio PlayerAudio{};
};