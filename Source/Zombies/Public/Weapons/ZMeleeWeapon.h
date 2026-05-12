// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/ZWeapon.h"
#include "ZMeleeWeapon.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class USceneComponent;

/**
 * 
 */
UCLASS()
class ZOMBIES_API AZMeleeWeapon : public AZWeapon
{
	GENERATED_BODY()
	
public:
	AZMeleeWeapon();

	virtual UMeshComponent* GetMesh() const override { return Mesh; }

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root;
};

