// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "GameplayTagContainer.h"
#include "UObject/ObjectKey.h"
#include "ZCharacterEditorUtilityWidget.generated.h"

class AZCharacter;
class UZGameplayTagPickerWidget;
class UStackBox;
class UEditorUtilityCheckBox;

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class ZOMBIESEDITOR_API UZCharacterEditorUtilityWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	void OnBeginPIE(const bool bIsSimulating);
	void OnEndPIE(const bool bIsSimulating);
	
	UFUNCTION()
	void OnCharacterDestroyed(AActor* DestroyedActor);

	void OnSelectionChanged(UObject* NewSelection);

	void OnCharacterSelected(AZCharacter* Character);
	void OnCharacterUnselected(AZCharacter* Character);

	UFUNCTION()
	void OnTagSelectionChanged(const FGameplayTagContainer& NewSelection);
	
	void AddAllSelectedNPCs(UWorld* World);
	void RemoveNPCs(const FGameplayTagContainer& Removed);
	
	UFUNCTION()
	void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);
	
	UWorld* GetCurrentWorld() const;
	
	TMap<FObjectKey, TObjectPtr<UWidget>> CharacterWidgets;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UZGameplayTagPickerWidget> TagPicker;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UStackBox> CharacterBox;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditorUtilityCheckBox> ShowSelf;
	
	FGameplayTagContainer SelectedNPCs;
	
	TWeakObjectPtr<APlayerController> PC;
	
	bool bPIE;
};

