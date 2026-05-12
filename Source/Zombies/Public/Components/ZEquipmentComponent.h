// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ZPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Inventory/ZEquippableItem.h"
#include "ZEquipmentComponent.generated.h"

class UInputAction;
class AZCharacter;
class AZEquippableItem;
enum class EZEquipType : uint8;
enum class EZItemState : uint8;

DECLARE_MULTICAST_DELEGATE_OneParam(FZOnEquip, AZEquippableItem*);
DECLARE_MULTICAST_DELEGATE_OneParam(FZOnUnequip, AZEquippableItem*);

UCLASS( ClassGroup=(Custom) )
class ZOMBIES_API UZEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UZEquipmentComponent();

	void StartEquip(TSubclassOf<AZEquippableItem> ItemClass);

	void StartUnequip();

	void EquipComplete();

	void UnequipComplete();

	UFUNCTION(BlueprintPure)
	AZEquippableItem* GetEquippedItem() const { return EquippedItem; }

	EZEquipType GetEquippedItemType() const;

	EZItemState GetEquippedItemState() const;

	void ClearEquippedItem() { EquippedItem = nullptr; }
	
	void SetupPlayerInputComponent(UEnhancedInputComponent* EnhancedInputComponent);

	template <EZItemInput Mode>
	void FirePressed();

	template <EZItemInput Mode>
	void FireReleased();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FZOnEquip OnEquip;

	FZOnEquip OnEquipComplete; // server only

	FZOnUnequip OnUnequip;

	FZOnUnequip OnUnequipComplete; // server only

protected:
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void Server_Reliable_StartEquip(TSubclassOf<AZEquippableItem> ItemClass);
	
	UPROPERTY(ReplicatedUsing = OnRep_EquippedItem)
	TObjectPtr<AZEquippableItem> EquippedItem;

	UFUNCTION()
	void OnRep_EquippedItem(AZEquippableItem* OldItem);

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> FireAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> ReloadAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> ADSAction;

	TSubclassOf<AZEquippableItem> PendingEquipItemClass; // #ZTODO is this set correctly on client/server?

	UPROPERTY()
	TObjectPtr<AZCharacter> CharacterOwner;
};

template <EZItemInput Mode>
void UZEquipmentComponent::FirePressed()
{
	if (EquippedItem)
	{
		EquippedItem->FirePressed(Mode);
	}
}

template <EZItemInput Mode>
void UZEquipmentComponent::FireReleased()
{
	if (EquippedItem)
	{
		EquippedItem->FireReleased(Mode);
	}
}

