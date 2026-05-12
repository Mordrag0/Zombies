// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "ZDelegateConnection.h"
#include "ZAmmoWidget.generated.h"

class UCommonTextBlock;
class AZFPCharacter;
class AZRangedWeapon;

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API UZAmmoWidget : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	void Init(AZFPCharacter* InCharacter);

protected:
	UFUNCTION()
	void OnEquip(AZEquippableItem* Item);

	UFUNCTION()
	void OnUnequip(AZEquippableItem* Item);

	UFUNCTION()
	void Update();

	void Reset();

	virtual void NativeDestruct() override;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> MagAmmo;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> TotalAmmo;

	TWeakObjectPtr<AZFPCharacter> WeakCharacter;

	UPROPERTY()
	TObjectPtr<AZRangedWeapon> RangedWeapon;

	FZDelegateConnection OnEquipConnection;
	FZDelegateConnection OnUnequipConnection;
	FZDelegateConnection OnAmmoChangedConnection;
};

