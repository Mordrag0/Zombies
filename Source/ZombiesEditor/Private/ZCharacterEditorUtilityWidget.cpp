// Copyright 2026 Luka Markuš. All rights reserved.


#include "ZCharacterEditorUtilityWidget.h"
#include "EditorUtilitySubsystem.h"
#include "EditorUtilityWidgetComponents.h"
#include "EngineUtils.h"
#include "LevelEditor.h"
#include "Selection.h"
#include "SZCharacterStateWidget.h"
#include "ZGameInstance.h"
#include "ZGameplayTagPickerWidget.h"
#include "ZGameplayTags.h"
#include "Characters/ZCharacter.h"
#include "Characters/ZNPCharacter.h"
#include "Components/CheckBox.h"
#include "Components/NativeWidgetHost.h"
#include "Components/PanelWidget.h"
#include "Components/StackBox.h"
#include "Components/StackBoxSlot.h"
#include "Editor/LevelEditor/Private/SLevelEditor.h"


void UZCharacterEditorUtilityWidget::NativeConstruct()
{
	Super::NativeConstruct();

	FEditorDelegates::PostPIEStarted.AddUObject(this, &ThisClass::OnBeginPIE);
	FEditorDelegates::EndPIE.AddUObject(this, &ThisClass::OnEndPIE);
	GEditor->GetSelectedActors()->SelectionChangedEvent.AddUObject(this, &ThisClass::OnSelectionChanged);
	if (TagPicker)
	{
		TagPicker->OnTagSelectionChanged.AddDynamic(this, &ThisClass::OnTagSelectionChanged);
		TagPicker->SetRootTag(ZGameplayTags::NPC);
		TagPicker->SetMultiSelect(true);
	}
}

void UZCharacterEditorUtilityWidget::NativeDestruct()
{
	Super::NativeDestruct();

	FEditorDelegates::PostPIEStarted.RemoveAll(this);
	FEditorDelegates::EndPIE.RemoveAll(this);
	GEditor->GetSelectedActors()->SelectionChangedEvent.RemoveAll(this);
	if (TagPicker)
	{
		TagPicker->OnTagSelectionChanged.RemoveAll(this);
	}
}

void UZCharacterEditorUtilityWidget::OnBeginPIE(const bool bIsSimulating)
{
	bPIE = true;
	RemoveNPCs(SelectedNPCs);
	AddAllSelectedNPCs(GetCurrentWorld());
	if (ShowSelf->IsChecked())
	{
		PC = GetCurrentWorld()->GetFirstPlayerController();
		if (PC.IsValid())
		{
			OnCharacterSelected(PC->GetPawn<AZCharacter>());
			PC->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::OnPossessedPawnChanged);
		}
	}
}

void UZCharacterEditorUtilityWidget::OnEndPIE(const bool bIsSimulating)
{
	if (PC.IsValid())
	{
		OnCharacterUnselected(PC->GetPawn<AZCharacter>());
		PC->OnPossessedPawnChanged.RemoveAll(this);
		PC.Reset();
	}
	bPIE = false;
	RemoveNPCs(SelectedNPCs);
	AddAllSelectedNPCs(GetCurrentWorld());
}

void UZCharacterEditorUtilityWidget::OnCharacterDestroyed(AActor* DestroyedActor)
{
	if (AZCharacter* Character = Cast<AZCharacter>(DestroyedActor))
	{
		OnCharacterUnselected(Character);
	}
}

void UZCharacterEditorUtilityWidget::OnSelectionChanged(UObject* NewSelection)
{
	TArray<AZCharacter*> SelectedCharacters;
	if (AZCharacter* SelectedCharacter = Cast<AZCharacter>(NewSelection))
	{
		SelectedCharacters.Add(Cast<AZCharacter>(NewSelection));
	}
	else if (USelection* Selection = Cast<USelection>(NewSelection))
	{
		Selection->GetSelectedObjects<AZCharacter>(SelectedCharacters);
	}
	
	TArray<FObjectKey> WeakSelectedCharacters;
	CharacterWidgets.GetKeys(WeakSelectedCharacters);
	TArray<AZCharacter*> PreviouslySelectedCharacters;
	for (const FObjectKey& ObjectKey : WeakSelectedCharacters)
	{
		PreviouslySelectedCharacters.Add(Cast<AZCharacter>(ObjectKey.GetWeakObjectPtr().Get()));
	}
	TArray<AZCharacter*> NewlyAddedCharacters = SelectedCharacters;
	NewlyAddedCharacters.RemoveAll([PreviouslySelectedCharacters](AZCharacter* Character)
	{
		return PreviouslySelectedCharacters.Contains(Character);
	});
	for (AZCharacter* Character : NewlyAddedCharacters)
	{
		OnCharacterSelected(Character);
	}
	TArray<AZCharacter*> NewlyRemovedCharacters = PreviouslySelectedCharacters;
	NewlyRemovedCharacters.RemoveAll([this, SelectedCharacters](AZCharacter* Character)
	{
		if (AZNPCharacter* NPC = Cast<AZNPCharacter>(Character))
		{
			return SelectedNPCs.HasTagExact(NPC->GetFullNPCName());
		}
		return SelectedCharacters.Contains(Character);
	});
	for (AZCharacter* Character : NewlyRemovedCharacters)
	{
		OnCharacterUnselected(Character);
	}
}

void UZCharacterEditorUtilityWidget::OnCharacterSelected(AZCharacter* Character)
{
	if (!Character)
	{
		return;
	}
	if (CharacterWidgets.Contains(Character))
	{
		return;
	}
	UNativeWidgetHost* Host = NewObject<UNativeWidgetHost>(this);
	Host->SetContent(SNew(SZCharacterStateWidget).Character(Character));
	if (UStackBoxSlot* SBSlot = CharacterBox->AddChildToStackBox(Host))
	{
		SBSlot->SetSize(ESlateSizeRule::Fill);
	}
	CharacterWidgets.Emplace(Character, Host);
	Character->OnDestroyed.AddDynamic(this, &ThisClass::OnCharacterDestroyed);
}

void UZCharacterEditorUtilityWidget::OnCharacterUnselected(AZCharacter* Character)
{
	if (!Character)
	{
		return;
	}
	CharacterBox->RemoveChild(CharacterWidgets[Character]);
	CharacterWidgets.Remove(Character);
	Character->OnDestroyed.RemoveAll(this);
}

void UZCharacterEditorUtilityWidget::OnTagSelectionChanged(const FGameplayTagContainer& NewSelection)
{
	FGameplayTagContainer Removed = SelectedNPCs;
	Removed.RemoveTags(NewSelection);
	SelectedNPCs = NewSelection;
	RemoveNPCs(Removed);
	AddAllSelectedNPCs(GetCurrentWorld());
}

void UZCharacterEditorUtilityWidget::AddAllSelectedNPCs(UWorld* World)
{
	for (TActorIterator<AZNPCharacter> It(World); It; ++It)
	{
		if (SelectedNPCs.HasTagExact(It->GetFullNPCName()))
		{
			OnCharacterSelected(*It);
		}
	}
}

void UZCharacterEditorUtilityWidget::RemoveNPCs(const FGameplayTagContainer& Removed)
{
	TArray<FObjectKey> Characters;
	CharacterWidgets.GetKeys(Characters);
	for (FObjectKey CharacterKey : Characters)
	{
		if (AZNPCharacter* NPC = Cast<AZNPCharacter>(CharacterKey.GetWeakObjectPtr().Get()))
		{
			if (Removed.HasTagExact(NPC->GetFullNPCName()))
			{
				OnCharacterUnselected(NPC);
			}
		}
	}
}

void UZCharacterEditorUtilityWidget::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	OnCharacterUnselected(Cast<AZCharacter>(OldPawn));
	OnCharacterSelected(Cast<AZCharacter>(NewPawn));
}

UWorld* UZCharacterEditorUtilityWidget::GetCurrentWorld() const
{
	if (!bPIE)
	{
		return GetWorld();
	}
	return GEditor->GetPIEWorldContext()->World();
}

