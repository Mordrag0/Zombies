// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/ZWeapon.h"
#include "ZDelegateConnection.h"
#include "ZRangedWeapon.generated.h"

class USkeletalMeshComponent;
class USceneComponent;
class AZInventoryItem;
class AZAmmo;
class UCurveFloat;

DECLARE_MULTICAST_DELEGATE(FZAmmoChanged);
DECLARE_MULTICAST_DELEGATE(FZOnReload);

/**
 * 
 */
UCLASS()
class ZOMBIES_API AZRangedWeapon : public AZWeapon
{
	GENERATED_BODY()
	
public:
	AZRangedWeapon();

	virtual UMeshComponent* GetMesh() const override { return Mesh; }

	virtual void PerformShot() override;

	virtual bool CanFire() const;

	int32 GetAmmoInMag() const { return FMath::Max(AmmoInMag - PendingShots, 0); }
	int32 GetAmmoPerMag() const { return AmmoPerMag; }
	FPrimaryAssetId GetAmmoType() const { return AmmoType; }

	virtual void Reload();

	virtual void OnEquip(AZCharacter* InOwner) override;
	virtual void UnequipComplete() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure)
	bool CanReload() const;

	UFUNCTION(BlueprintPure)
	bool NeedsReload() const { return GetAmmoInMag() == 0; }

	void StartReload(float Duration);

	bool OwnerHasAmmo() const;

	virtual void Tick(float DeltaSeconds) override;

	FZAmmoChanged OnAmmoChanged;
	FZAmmoChanged OnOutOfAmmo;
	FZOnReload OnReload; // Server only

protected:
	virtual void ConsumeAmmo();
	
	virtual void PlayFireEffects() override;

	virtual void StopFireEffects() override;

	UFUNCTION()
	void OnOwnerInventoryChanged();

	virtual void BeginPlay() override;

	void AddRecoil();

	UFUNCTION(Server, Reliable)
	void Server_Reliable_StartReload(float Duration);

	UPROPERTY(EditDefaultsOnly, Category = "Ranged Weapon")
	int32 AmmoPerMag;

	UPROPERTY(ReplicatedUsing = OnRep_AmmoInMag)
	int32 AmmoInMag;

	int32 PendingShots;

	UFUNCTION()
	void OnRep_AmmoInMag(int32 OldAmmo);

	UPROPERTY(EditDefaultsOnly)
	FPrimaryAssetId AmmoType;
	
	UPROPERTY(EditDefaultsOnly, Category = "Ranged Weapon")
	TObjectPtr<UCurveFloat> RecoilX;
	
	UPROPERTY(EditDefaultsOnly, Category = "Ranged Weapon")
	TObjectPtr<UCurveFloat> RecoilY;

	bool bRecoil;
	float TimeOfLastShot;
	bool bRecoilXDirection;
	float RecoilTime;

	FZDelegateConnection OnInventoryChangedConnection;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> Root;
};

