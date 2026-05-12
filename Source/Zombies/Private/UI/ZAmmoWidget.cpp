// Copyright 2026 Luka Markuš. All rights reserved.


#include "UI/ZAmmoWidget.h"
#include "Characters/ZFPCharacter.h"
#include "Weapons/ZRangedWeapon.h"
#include "CommonTextBlock.h"
#include "Inventory/ZInventoryComponent.h"
#include "Components/ZEquipmentComponent.h"

void UZAmmoWidget::Init(AZFPCharacter* InCharacter)
{
	WeakCharacter = InCharacter;
	UZEquipmentComponent* EquipmentComp = InCharacter->GetEquipmentComponent();
	BIND_MULTICAST_UOBJECT(OnEquipConnection, EquipmentComp, EquipmentComp->OnEquip, this, &ThisClass::OnEquip);
	BIND_MULTICAST_UOBJECT(OnUnequipConnection, EquipmentComp, EquipmentComp->OnUnequip, this, &ThisClass::OnUnequip);
	OnEquip(InCharacter->GetEquipmentComponent()->GetEquippedItem());
}

void UZAmmoWidget::OnEquip(AZEquippableItem* Item)
{
	if (!WeakCharacter.IsValid())
	{
		return;
	}
	RangedWeapon = Cast<AZRangedWeapon>(Item);
	if (RangedWeapon)
	{
		MagAmmo->SetText(FText::AsNumber(RangedWeapon->GetAmmoInMag()));
		TotalAmmo->SetText(FText::AsNumber(WeakCharacter->GetInventoryComponent()->GetItemCount(RangedWeapon->GetAmmoType())));
		BIND_MULTICAST_UOBJECT(OnAmmoChangedConnection, RangedWeapon, RangedWeapon->OnAmmoChanged, this, &ThisClass::Update);
		SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		Reset();
	}
}

void UZAmmoWidget::OnUnequip(AZEquippableItem* Item)
{
	OnAmmoChangedConnection.Disconnect();
	Reset();
}

void UZAmmoWidget::Update()
{
	if (!WeakCharacter.IsValid())
	{
		return;
	}
	MagAmmo->SetText(FText::AsNumber(RangedWeapon->GetAmmoInMag()));
	TotalAmmo->SetText(FText::AsNumber(WeakCharacter->GetInventoryComponent()->GetItemCount(RangedWeapon->GetAmmoType())));
}

void UZAmmoWidget::Reset()
{
	MagAmmo->SetText(INVTEXT("---"));
	TotalAmmo->SetText(INVTEXT("---"));
	SetVisibility(ESlateVisibility::Hidden);
}

void UZAmmoWidget::NativeDestruct()
{
	OnEquipConnection.Disconnect();
	OnUnequipConnection.Disconnect();
	WeakCharacter.Reset();
	Super::NativeDestruct();
}

