// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/ZRangedWeapon.h"
#include "ZHitScanWeapon.generated.h"

class UParticleSystem;

/**
 * 
 */
UCLASS()
class ZOMBIES_API AZHitScanWeapon : public AZRangedWeapon
{
	GENERATED_BODY()
	
public:
	AZHitScanWeapon();

	virtual void PerformShot() override;

protected:
	void PerformTrace();

	void PlayTraceEffects(const FVector& HitLocation);

	void TraceHit(const FHitResult& HitResult);

	UFUNCTION(Server, Unreliable)
	void Server_Unreliable_TraceNoHit(const FVector& EndLocation);

	UFUNCTION(Server, Unreliable)
	void Server_Reliable_TraceHit(const FHitResult& HitResult);

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_Unreliable_PlayTraceEffects(const FVector& HitLocation);

	float RecalculateSpread() const;

	FVector GetTraceDirection(float Spread, const FRotator& ViewDirection) const;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* TraceEffect;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float Damage;	

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float MinRangeDamage;	

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float MaxTraceRange;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float MinRange;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float MaxRange;
};

