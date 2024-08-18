// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterAttributeModule.h"
#include "Logging.h"

#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FCharacterAttributeModule"

void FCharacterAttributeModule::StartupModule()
{
}

void FCharacterAttributeModule::ShutdownModule()
{
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCharacterAttributeModule, CharacterAttributeModule);