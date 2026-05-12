// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

class UStateTree;

DECLARE_LOG_CATEGORY_EXTERN(LogZEditor, Log, All);

class FZombiesEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

protected:
	void OnPackageSaved(const FString& PackageFileName, UPackage* Package, FObjectPostSaveContext ObjectSaveContext);

	void RunValidatorOnStateTree(UStateTree* StateTree);

	void RegisterMenus();

	void OpenEventEditor();

	TSharedRef<SDockTab> SpawnEventEditorTab(const FSpawnTabArgs& Args);

	static const FName EventEditorTabName;
};
