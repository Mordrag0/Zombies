// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/ZEquippableItem.h"
#include "ZGrenade.generated.h"

class USkeletalMeshComponent;
class UProjectileMovementComponent;
class USceneComponent;
class AZExplosionEffect;

/**
 * 
 */
UCLASS()
class ZOMBIES_API AZGrenade : public AZEquippableItem
{
	GENERATED_BODY()
	
public:
	AZGrenade();

	virtual void FirePressed(EZItemInput Mode);
	virtual void FireReleased(EZItemInput Mode);

	void Release();

	virtual UMeshComponent* GetMesh() const override { return Mesh; }

protected:
	void StartHolding();

	UFUNCTION(Server, Reliable)
	void Server_Reliable_StartHolding();

	void StartThrowing();

	UFUNCTION(Server, Reliable)
	void Server_Reliable_StartThrowing();

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_Unreliable_PlayExplosionEffects();

	UFUNCTION()
	void DestroyGrenade();

	UFUNCTION()
	void Explode();

	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	float FuseTime;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	float ReleaseTime;

	FTimerHandle TimerHandle_Fuse;
	FTimerHandle TimerHandle_Destroy;
	
	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	float DamageInnerRadius;
	
	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	float DamageOuterRadius;
	
	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	float Damage;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	float DamageFalloff;
	
	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	float MinimumDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	TSubclassOf<AZExplosionEffect> ExplosionEffect;

	bool bReleased;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> ExplosionPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;
};

