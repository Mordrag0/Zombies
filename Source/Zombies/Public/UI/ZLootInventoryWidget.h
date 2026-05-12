// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "ZLootInventoryWidget.generated.h"

class UZInventoryComponent;
class UZInventoryWidget;

UENUM()
enum class EZLootWidgetType : uint8
{
	Player,
	Container
};

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API UZLootInventoryWidget : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	void Refresh(UZInventoryComponent* Inventory);

	void Refresh();

	EZLootWidgetType GetType() const { return Type; }

	UZInventoryWidget* GetInventoryWidget() const { return InventoryWidget; }

protected:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UZInventoryWidget> InventoryWidget;

	UPROPERTY(EditAnywhere)
	EZLootWidgetType Type;
};

