// Copyright 2026 Luka MarkuÃ…Â¡ All rights reserved.


#include "ZombiesEditorModule.h"
#include "UObject/Package.h"
#include "UObject/ObjectSaveContext.h"
#include "StateTree.h"
#include "ZStateTreeValidator.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/DataValidation.h"
#include "Logging/MessageLog.h"
#include "ToolMenuSection.h"
#include "ToolMenu.h"
#include "Widgets/Docking/SDockTab.h"
#include "SZEventEditor.h"
#include "ToolMenus.h"

IMPLEMENT_GAME_MODULE(FZombiesEditorModule, ZombiesEditor);

#define LOCTEXT_NAMESPACE "ZombiesEditor"

DEFINE_LOG_CATEGORY(LogZEditor)

const FName FZombiesEditorModule::EventEditorTabName(FName("ZEventEditor"));

void FZombiesEditorModule::StartupModule()
{
	UPackage::PackageSavedWithContextEvent.AddRaw(this, &FZombiesEditorModule::OnPackageSaved);
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FZombiesEditorModule::RegisterMenus));
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		EventEditorTabName,
		FOnSpawnTab::CreateRaw(this, &FZombiesEditorModule::SpawnEventEditorTab))
		.SetDisplayName(NSLOCTEXT("ZombiesEditor", "EventEditor", "Event Editor"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FZombiesEditorModule::ShutdownModule()
{
	UPackage::PackageSavedWithContextEvent.RemoveAll(this);
}

void FZombiesEditorModule::OnPackageSaved(const FString& PackageFileName, UPackage* Package, FObjectPostSaveContext ObjectSaveContext)
{
	if (ObjectSaveContext.IsProceduralSave() || ObjectSaveContext.IsCooking())
	{
		return;
	}
	const FAssetRegistryModule& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	// Find all State Trees that reference the saved package
	TArray<FAssetIdentifier> Referencers;
	AssetRegistry.Get().GetReferencers(FAssetIdentifier(Package->GetFName()), Referencers);

	for (const FAssetIdentifier& Referencer : Referencers)
	{
		FARFilter Filter;
		Filter.ClassPaths.Add(UStateTree::StaticClass()->GetClassPathName());
		Filter.PackageNames.Add(Referencer.PackageName);

		TArray<FAssetData> Assets;
		AssetRegistry.Get().GetAssets(Filter, Assets);

		for (const FAssetData& AssetData : Assets)
		{
			if (UStateTree* StateTree = Cast<UStateTree>(AssetData.GetAsset()))
			{
				RunValidatorOnStateTree(StateTree);
			}
		}
	}
}

void FZombiesEditorModule::RunValidatorOnStateTree(UStateTree* StateTree)
{
	UZStateTreeValidator* Validator = NewObject<UZStateTreeValidator>();
	FDataValidationContext ValidationContext;
	Validator->ValidateLoadedAsset(FAssetData(StateTree), StateTree, ValidationContext);

	FMessageLog StateTreeLog("AssetCheck");
	for (const FDataValidationContext::FIssue& Issue : ValidationContext.GetIssues())
	{
		TSharedRef<FTokenizedMessage> Message = FTokenizedMessage::Create(Issue.Severity);
		for (TSharedRef<IMessageToken> MessageToken : Issue.TokenizedMessage->GetMessageTokens())
		{
			Message->AddToken(MessageToken);
		}
		StateTreeLog.AddMessage(Message);
	}
	if (StateTreeLog.NumMessages(EMessageSeverity::Warning) > 0)
	{
		StateTreeLog.Open();
	}
}

void FZombiesEditorModule::RegisterMenus()
{
	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Tools");
	FToolMenuSection& Section = Menu->AddSection("ZombiesEditor", LOCTEXT("ZombiesEditor", "Zombies"));
	Section.AddMenuEntry(
		"OpenEventEditor",
		LOCTEXT("OpenEventEditor", "Event Editor"),
		LOCTEXT("OpenEventEditorTooltip", "Open the Event Editor"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &FZombiesEditorModule::OpenEventEditor))
	);
}

void FZombiesEditorModule::OpenEventEditor()
{
    FGlobalTabmanager::Get()->TryInvokeTab(EventEditorTabName);
}

TSharedRef<SDockTab> FZombiesEditorModule::SpawnEventEditorTab(const FSpawnTabArgs& Args)
{
    return SNew(SDockTab)
        .TabRole(ETabRole::NomadTab)
        [
            SNew(SZEventEditor) // Your main widget
        ];
}

#undef LOCTEXT_NAMESPACE

