// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZExplosionEffect.generated.h"

class USceneComponent;
class UPointLightComponent;
class USoundCue;

UCLASS()
class ZOMBIES_API AZExplosionEffect : public AActor
{
	GENERATED_BODY()
	
public:	
	AZExplosionEffect();

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion effect")
	TObjectPtr<UParticleSystem> VFX;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion effect")
	TObjectPtr<USoundCue> ExplosionSound;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion effect")
	float LightIntensity;
	
	/** How long keep explosion light on? */
	UPROPERTY(EditDefaultsOnly, Category = "Explosion effect")
	float ExplosionLightFadeOut;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UPointLightComponent* PointLightComponent;
};

