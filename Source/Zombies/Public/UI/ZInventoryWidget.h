// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Components/UniformGridPanel.h"
#include "ZInventoryWidget.generated.h"

class UUniformGridPanel;
class UZInventoryItemWidget;
class UZInventoryComponent;
class UZItemCountWidget;
class UCommonTextBlock;

/**
 * Displays the contents of a UZInventoryComponent
 */
UCLASS(Abstract)
class ZOMBIES_API UZInventoryWidget : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	UZInventoryWidget(const FObjectInitializer& ObjectInitializer);

	void Refresh(UZInventoryComponent* InInventory);

	void Refresh();

	template<typename T>
	TArray<T*> GetAllItemWidgets() const;

	UZInventoryComponent* GetInventoryComponent() const { return Inventory; }

protected:
	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> InventoryList;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UZInventoryItemWidget> ItemWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCommonUserWidget> EmptyItemWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	uint32 NumCols;

	UPROPERTY(EditDefaultsOnly)
	uint32 TotalCount;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidgetOptional))
	TObjectPtr<UZItemCountWidget> CountWidget;

	UPROPERTY()
	TObjectPtr<UZInventoryComponent> Inventory;
};

template<typename T>
TArray<T*> UZInventoryWidget::GetAllItemWidgets() const
{
	TArray<T*> Ret;
	TArray<UWidget*> Widgets = InventoryList->GetAllChildren();
	for (UWidget* Widget : Widgets)
	{
		if (T* RetWidget = Cast<T>(Widget))
		{
			Ret.Add(RetWidget);
		}
	}
	return Ret;
}

