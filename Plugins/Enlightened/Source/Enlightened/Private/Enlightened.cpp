// Copyright Epic Games, Inc. All Rights Reserved.

#include "Enlightened.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FEnlightenedModule"

void FEnlightenedModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	UE_LOG(LogTemp, Warning, TEXT("Enlightened module STARTUP"));
}

void FEnlightenedModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	UE_LOG(LogTemp, Warning, TEXT("Enlightened module SHUTDOWN"));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FEnlightenedModule, Enlightened)