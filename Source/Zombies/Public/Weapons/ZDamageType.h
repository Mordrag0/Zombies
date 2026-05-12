// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/DamageType.h"
#include "ZDamageType.generated.h"

USTRUCT(BlueprintType)
struct FZPointDamageEvent : public FPointDamageEvent
{
	GENERATED_BODY()

	FZPointDamageEvent() { }
	FZPointDamageEvent(float InDamage, struct FHitResult const& InHitInfo, FVector const& InShotDirection, TSubclassOf<class UDamageType> InDamageTypeClass)
		: FPointDamageEvent(InDamage, InHitInfo, InShotDirection, InDamageTypeClass)
	{
	}

	/** ID for this class. NOTE this must be unique for all damage events. */
	static const int32 ClassID = 101;

	virtual int32 GetTypeID() const override { return FZPointDamageEvent::ClassID; }

	virtual bool IsOfType(int32 InID) const override { return (FZPointDamageEvent::ClassID == InID) || FPointDamageEvent::IsOfType(InID); }
};

USTRUCT(BlueprintType)
struct FZRadialDamageEvent : public FRadialDamageEvent
{
	GENERATED_BODY()

	FZRadialDamageEvent() {}

	/** ID for this class. NOTE this must be unique for all damage events. */
	static const int32 ClassID = 102;

	virtual int32 GetTypeID() const override { return FZRadialDamageEvent::ClassID; }

	virtual bool IsOfType(int32 InID) const override { return (FZRadialDamageEvent::ClassID == InID) || FRadialDamageEvent::IsOfType(InID); }
};


USTRUCT(BlueprintType)
struct FZFallDamageEvent : public FDamageEvent
{
	GENERATED_BODY()

	FZFallDamageEvent() : Velocity(0.f) {}
	FZFallDamageEvent(float InVelocity, TSubclassOf<class UDamageType> InDamageTypeClass) : FDamageEvent(InDamageTypeClass), Velocity(InVelocity) {}

	float Velocity;

	/** ID for this class. NOTE this must be unique for all damage events. */
	static const int32 ClassID = 103;

	virtual int32 GetTypeID() const override { return FZFallDamageEvent::ClassID; }

	virtual bool IsOfType(int32 InID) const override { return (FZFallDamageEvent::ClassID == InID); }
};

/**
 * 
 */
UCLASS(const, Blueprintable, BlueprintType)
class UZDamageType : public UDamageType
{
	GENERATED_BODY()

public:
};

