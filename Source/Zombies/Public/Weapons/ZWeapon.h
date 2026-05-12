// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/ZEquippableItem.h"
#include "ZWeapon.generated.h"

class USoundCue;
class AZCharacter;
class UZWeaponAction;
class AZImpactEffect;
class UParticleSystem;
class UParticleSystemComponent;
class UDamageType;
class UAudioComponent;
enum class EZItemInput : uint8;

UCLASS()
class ZOMBIES_API AZWeapon : public AZEquippableItem
{
	GENERATED_BODY()
	
public:	
	virtual void FirePressed(EZItemInput Mode) override;
	virtual void FireReleased(EZItemInput Mode) override;

	virtual void OnEquip(AZCharacter* InOwner) override;

	virtual void UnequipComplete() override;

	virtual void PerformShot();

	void FireComplete();

	virtual bool CanFire() const;

protected:
	virtual void BeginPlay() override;

	void SpawnImpactEffect(const FHitResult& Impact);

	virtual void PlayFireEffects();

	virtual void StopFireEffects();

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_Unreliable_PlayFireEffects();

	UFUNCTION(Server, Reliable)
	void Server_Reliable_PerformShot();

	UFUNCTION(Server, Reliable)
	void Server_Reliable_FireComplete();

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<AZImpactEffect> ImpactEffect; 
	
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TObjectPtr<UParticleSystem> MuzzleEffect;
	
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	FName MuzzleSocketName;

	UPROPERTY()
	TObjectPtr<UParticleSystemComponent> MuzzleParticleComp;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TObjectPtr<USoundCue> FireSound; 
	
	UPROPERTY()
	TObjectPtr<UAudioComponent> FireSoundComp;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	USoundAttenuation* AttenuationSettings;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	USoundAttenuation* AttenuationSettings3P;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	USoundConcurrency* ConcurrencySettings;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	USoundConcurrency* ConcurrencySettings3P;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY()
	TMap<EZItemInput, UZWeaponAction*> Actions;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TMap<EZItemInput, TSubclassOf<UZWeaponAction>> ActionClassList;

private:
	void InitWeaponActions();

	void DestroyWeaponActions();

	bool bWeaponActionsInitialized;
};

