#include "ProtoEnginePCH.h"
#include "Audio.h"
Audio* ServiceLocator::audioService = nullptr;
NullAudio ServiceLocator::defaultAudioService;