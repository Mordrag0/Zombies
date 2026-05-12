// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "ZEventEditorSettings.generated.h"

class UDataTable;

/**
 * 
 */
UCLASS(Config = Editor, defaultconfig)
class ZOMBIESEDITOR_API UZEventEditorSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
    virtual FName GetCategoryName() const override { return "Project"; }
    virtual FName GetSectionName() const override { return "Event Editor"; }

	TArray<FSoftObjectPath> GetEventsDataTables() const { return EventsDataTables; }
	TArray<FSoftObjectPath> GetDialogueOptionsDataTables() const { return DialogueOptionsDataTables; }

    UPROPERTY(Config, EditAnywhere, Category = "Colors")
    FLinearColor DirectEventColor = FLinearColor::White;

    UPROPERTY(Config, EditAnywhere, Category = "Colors")
    FLinearColor IndirectEventColor = FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);

private:
    UPROPERTY(Config, EditAnywhere, Category = "Event Editor", meta=(AllowedClasses="/Script/Engine.DataTable"))
    TArray<FSoftObjectPath> EventsDataTables;

    UPROPERTY(Config, EditAnywhere, Category = "Event Editor", meta=(AllowedClasses="/Script/Engine.DataTable"))
    TArray<FSoftObjectPath> DialogueOptionsDataTables;
};

