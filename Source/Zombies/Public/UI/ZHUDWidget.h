// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZActivatableWidget.h"
#include "GameplayTagContainer.h"
#include "ZDelegateConnection.h"
#include "ZHUD.h"
#include "ZHUDWidget.generated.h"

class UZInventoryWidget;
class AZCharacter;
class UZTradeWidget;
class UZInventoryComponent;
class UZLootWidget;
class AZNPCharacter;
class UZAmmoWidget;
class AZFPCharacter;
class UZHealthWidget;
class UZStaminaWidget;
struct FZDialogueOptionRow;
class UZQuestWidget;
class UZSkillWidget;
class UZMapWidget;
class UZLockpickWidget;
class UZPauseMenuWidget;
class AZLock;
class UZDialogueWidget;
class UZNotificationManagerWidget;
struct FZDialogueParams;
class UCommonActivatableWidget;
class AZHUD;
class UZPlayerInventoryWidget;
enum class EZNotificationType : uint8;

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API UZHUDWidget : public UZActivatableWidget
{
	GENERATED_BODY()
	
public:
	void Init(AZFPCharacter* InCharacter);

	void ShowInventory(UZInventoryComponent* Inventory);

	void StartDialogue(AZNPCharacter* NPC, const FZDialogueParams& DialogueParams);

	void StartDialogue(const FZDialogueOptionRow* NPCInitiatedRow);

	void PushDialogueOptions(AZNPCharacter* NPC, const FZDialogueParams& DialogueParams);

	void RefreshDialogueOptions();

	void ShowResponse(const FZDialogueOptionRow* Row);

	void EndDialogue();

	void StartTrade(AZNPCharacter* NPC);

	void UpdateTradeCost();

	void StartLooting(UZInventoryComponent* Inventory);

	void StopLooting();

	void ShowQuests();

	void ShowSkills();

	void ShowMap();

	void StartLockpicking(AZLock* Lock);

	void StopLockpicking();

	void ShowPauseWidget();

	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const;

protected:
	virtual void NativeDestruct() override;
	
	void ShowDialogueWidget();

	AZHUD* GetHUD() const;

	template<typename T> requires std::is_base_of_v<UZActivatableWidget, T>
	void ShowActivatableWidget(TObjectPtr<T>& Widget, TSubclassOf<T> WidgetClass, EZActivatableWidgetType WidgetType);

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UZPlayerInventoryWidget> InventoryWidget;

	UPROPERTY()
	TObjectPtr<UZDialogueWidget> DialogueWidget;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UZTradeWidget> TradeWidget;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UZLootWidget> LootWidget;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	TObjectPtr<UZAmmoWidget> AmmoWidget;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	TObjectPtr<UZHealthWidget> HealthBar;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	TObjectPtr<UZStaminaWidget> StaminaBar;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UZQuestWidget> QuestWidget;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UZSkillWidget> SkillsWidget;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UZMapWidget> MapWidget;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UZLockpickWidget> LockpickWidget;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UZPauseMenuWidget> PauseMenuWidget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UZDialogueWidget> DialogueWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UZPlayerInventoryWidget> InventoryWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UZMapWidget> MapWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UZPauseMenuWidget> PauseMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UZLootWidget> LootWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UZQuestWidget> QuestWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UZSkillWidget> SkillsWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UZLockpickWidget> LockpickWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UZTradeWidget> TradeWidgetClass;

	TMap<UClass*, FZDelegateConnection> WidgetDeactivatedConnections;
	FZDelegateConnection UnpauseConnection;
};

template<typename T> requires std::is_base_of_v<UZActivatableWidget, T>
void UZHUDWidget::ShowActivatableWidget(TObjectPtr<T>& Widget, TSubclassOf<T> WidgetClass, EZActivatableWidgetType WidgetType)
{
	if (Widget)
	{
		return; 
	}
	Widget = GetHUD()->PushActivatableWidget<T>(WidgetType, WidgetClass);
	FZDelegateConnection OnWidgetDeactivatedConnection;
	BIND_MULTICAST(OnWidgetDeactivatedConnection, Widget, Widget->OnRemoved, [this, &Widget]()
	{
		WidgetDeactivatedConnections.Remove(Widget->GetClass());
		Widget = nullptr;
	});
	WidgetDeactivatedConnections.Emplace(WidgetClass, MoveTemp(OnWidgetDeactivatedConnection));
}


