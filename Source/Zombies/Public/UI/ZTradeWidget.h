// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZActivatableWidget.h"
#include "ZDelegateConnection.h"
#include "ZTradeWidget.generated.h"

class AZNPCharacter;
class UZTradeInventoryWidget;
class UZButton;
class UCommonTextBlock;

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API UZTradeWidget : public UZActivatableWidget
{
	GENERATED_BODY()
	
public:
	void StartTrade(AZNPCharacter* InNPC);

	void UpdateCost();

	int32 GetCurrentCost() const;

protected:
	virtual void NativeOnInitialized() override;

	virtual void NativeOnDeactivated() override;

	UFUNCTION()
	void Accept();

	UFUNCTION()
	void Reset();

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	TObjectPtr<UZTradeInventoryWidget> PlayerInventory;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	TObjectPtr<UZTradeInventoryWidget> NPCInventory;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UZButton> AcceptButton;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UZButton> ResetButton;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UZButton> DeclineButton;

	UPROPERTY()
	TObjectPtr<AZNPCharacter> NPC;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> TradeCost;

	FZDelegateConnection OnPlayerInventoryChangedConnection;
	FZDelegateConnection OnNPCInventoryChangedConnection;
};

