// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZActivatableWidget.h"
#include "GameplayTagContainer.h"
#include "ZNetworkTypes.h"
#include "ZDialogueWidget.generated.h"

class UVerticalBox;
class UZDialogueRowWidget;
class UZDialogueResponseRowWidget;
class AZNPCharacter;
class UZTradeRowWidget;
struct FZDialogueOptionRow;
class UZEndDialogueRowWidget;
class UZTeachRowWidget;
struct FZDialogueParams;

enum class EZDialogueWidgetState : uint8
{
	None,
	WaitingForOptions,
	PendingOptions,
};

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API UZDialogueWidget : public UZActivatableWidget
{
	GENERATED_BODY()
	
public:
	void PushDialogueOptions(AZNPCharacter* NPC, const FZDialogueParams& DialogueParams);

	void ShowDialogueResponse(const FZDialogueOptionRow* Row);

	void RefreshDialogueOptions();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	TObjectPtr<UVerticalBox> VerticalBox;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UZDialogueRowWidget> DialogueRowClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UZTradeRowWidget> TradeRowClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UZTeachRowWidget> TeachRowClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UZEndDialogueRowWidget> EndRowClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UZDialogueResponseRowWidget> ResponseRowClass;

	EZDialogueWidgetState DialogueWidgetState;

	UPROPERTY()
	TObjectPtr<AZNPCharacter> PendingNPC;

	FZDialogueParams PendingDialogueParams;
};

