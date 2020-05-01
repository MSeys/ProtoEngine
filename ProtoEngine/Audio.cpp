#include "ProtoEnginePCH.h"
#include "Audio.h"
Audio* AudioServiceLocator::audioService = nullptr;
NullAudio AudioServiceLocator::defaultAudioService;