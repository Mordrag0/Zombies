// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZActivatableWidget.h"
#include "ZLootInventoryWidget.h"
#include "Inventory/ZInventoryComponent.h"
#include "ZDelegateConnection.h"
#include "ZLootWidget.generated.h"

class UZInventoryComponent;
class UZLootInventoryWidget;
class UZButton;

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API UZLootWidget : public UZActivatableWidget
{
	GENERATED_BODY()
	
public:
	void StartLooting(UZInventoryComponent* InInventory);

	void Move(FPrimaryAssetId ItemId, int32 Count, EZLootWidgetType Type);

	virtual void RemoveFromStack() override;

protected:
	virtual void NativeOnInitialized() override;

	virtual void NativeOnDeactivated() override;

	UFUNCTION()
	void TakeAll();

	UFUNCTION()
	void Refresh();

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	TObjectPtr<UZLootInventoryWidget> PlayerInventory;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	TObjectPtr<UZLootInventoryWidget> ContainerInventory;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UZButton> ExitButton;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UZButton> TakeAllButton;

	UPROPERTY()
	TObjectPtr<UZInventoryComponent> ContainerInventoryComponent;

	FZDelegateConnection OnPlayerInventoryChangedConnection;
	FZDelegateConnection OnContainerInventoryChangedConnection;
};

