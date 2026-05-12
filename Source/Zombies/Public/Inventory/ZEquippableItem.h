// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/ZInventoryItem.h"
#include "ZEquippableItem.generated.h"

class AZCharacter;
enum class EZEquipType : uint8;

DECLARE_MULTICAST_DELEGATE_OneParam(FZOnItemStateChanged, EZItemState);

/**
 * 
 */
UCLASS()
class ZOMBIES_API AZEquippableItem : public AZInventoryItem
{
	GENERATED_BODY()
	
public:
	AZEquippableItem();

	virtual bool CanBeEquipped() const override { return true; }

	FName GetAttachSocket() const { return AttachSocket; }

	EZItemState GetItemState() const { return HasAuthority() ? ItemState : PredictedState; }

	void SetItemState(EZItemState InItemState);

	bool IsDown() const;
	bool IsIdle() const;
	bool IsEquipping() const;
	bool IsFiring() const;
	bool IsHolding() const;
	bool IsUnequipping() const;
	bool IsReloading() const;

	virtual void OnEquip(AZCharacter* InOwner);

	virtual void Unequip();

	virtual void EquipComplete();

	virtual void UnequipComplete();
	
	EZEquipType GetEquipType() const { return EquipType; }

	virtual bool CanInteract(const AZCharacter* Character) const override;

	UFUNCTION(BlueprintCallable)
	virtual void FirePressed(EZItemInput Mode);

	UFUNCTION(BlueprintCallable)
	virtual void FireReleased(EZItemInput Mode);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void SetVisible(bool bInVisible);

	FZOnItemStateChanged OnItemStateChanged;

protected:
	virtual void BeginPlay() override;

	bool IsClientPredictedState(EZItemState InItemState) const;

	virtual void HandleItemStateChanged();

	UFUNCTION(Client, Reliable)
	void Client_Reliable_SetItemState(EZItemState InItemState);

	UFUNCTION(Server, Reliable)
	void Server_Reliable_Unequip();
	
	UPROPERTY(EditDefaultsOnly)
	EZEquipType EquipType;

	UPROPERTY(EditDefaultsOnly)
	FName AttachSocket;

	UPROPERTY(ReplicatedUsing = OnRep_ItemState)
	EZItemState ItemState;

	UFUNCTION()
	void OnRep_ItemState();

	EZItemState PredictedState;

	EZItemState PendingState;

	FTimerHandle TimerHandle_Action;
	FTimerHandle TimerHandle_Visibility;

	UPROPERTY(EditDefaultsOnly)
	float ShowWeaponDelay;

	UPROPERTY(EditDefaultsOnly)
	float EquipDuration;

	UPROPERTY(EditDefaultsOnly)
	float HideWeaponDelay;

	UPROPERTY(EditDefaultsOnly)
	float UnequipDuration;

	UPROPERTY(ReplicatedUsing = OnRep_Visible)
	bool bVisible;

	UFUNCTION()
	void OnRep_Visible();
};

