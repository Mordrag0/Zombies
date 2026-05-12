// Copyright 2026 Luka Markuš. All rights reserved.


#include "Weapons/ZHitScanWeapon.h"
#include "ZConstants.h"
#include "Characters/ZCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Weapons/ZDamageType.h"
#include "Weapons/ZFireMode.h"
#include "Weapons/ZADSAction.h"
#include "ZTypes.h"

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)

static TAutoConsoleVariable<int32> CVarShowTraceDebug(
	TEXT("z.ShowTraceDebug"),
	0,
	TEXT("Enables rendering debug lines for trace weapons.\n")
	TEXT("  0: off\n")
	TEXT("  1: on\n"),
	ECVF_Cheat);

#endif

AZHitScanWeapon::AZHitScanWeapon()
{
	Damage = 10.f;
	MinRangeDamage = 5.f;
	MaxTraceRange = 10000.f;
	MinRange = 1000.f;
	MaxRange = 7000.f;
}

void AZHitScanWeapon::PerformShot()
{
	Super::PerformShot();

	if (CharacterOwner->IsLocallyControlled())
	{
		PerformTrace();
	}
}

void AZHitScanWeapon::PerformTrace()
{
	const float Spread = RecalculateSpread();

	FVector StartLocation;
	FRotator ViewDirection;
	CharacterOwner->GetAimViewPoint(StartLocation, ViewDirection);
	const FVector TraceDirection = GetTraceDirection(Spread, ViewDirection);

	const FVector EndLocation = StartLocation + (TraceDirection * MaxTraceRange);;

	FCollisionQueryParams CQP = FCollisionQueryParams::DefaultQueryParam;
	CQP.AddIgnoredActor(this);
	CQP.AddIgnoredActor(CharacterOwner);
	CQP.bReturnPhysicalMaterial = true;
	CQP.bTraceComplex = true;

	FHitResult HitResult;
	if (!GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_WeaponTrace, CQP) || !HitResult.GetActor())
	{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		const int32 bShowDebug = CVarShowTraceDebug.GetValueOnGameThread();
		if (bShowDebug)
		{
			DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Yellow, false, 5.f);
		}
#endif
		PlayTraceEffects(EndLocation);
		Server_Unreliable_TraceNoHit(EndLocation);
	}
	else // Hit
	{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		const int32 bShowDebug = CVarShowTraceDebug.GetValueOnGameThread();
		if (bShowDebug)
		{
			DrawDebugLine(GetWorld(), StartLocation, HitResult.ImpactPoint, FColor::Red, false, 5.f);
		}
#endif
		SpawnImpactEffect(HitResult);
		PlayTraceEffects(HitResult.ImpactPoint);
		Server_Reliable_TraceHit(HitResult);
	}
}

void AZHitScanWeapon::PlayTraceEffects(const FVector& HitLocation)
{
	if (TraceEffect)
	{
		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TraceEffect, GetMesh()->GetSocketTransform(MuzzleSocketName));
		if (TracerComp)
		{
			TracerComp->SetVectorParameter("BeamEnd", HitLocation);
		}
	}
}

void AZHitScanWeapon::TraceHit(const FHitResult& HitResult)
{
	// Linear damage falloff
	const float Range = FVector::Distance(HitResult.TraceStart, HitResult.ImpactPoint);
	const float RangeModifier = FMath::Clamp((Range - MinRange) / (MaxRange - MinRange), 0.f, 1.f);
	const float ActualDamage = Damage * (1 - RangeModifier) + MinRangeDamage * RangeModifier;
	//UE_LOG(LogZWeapon, Log, TEXT("%f, %f, %f"), Range, RangeModifier, ActualDamage);
	FZPointDamageEvent PointDamageEvent = FZPointDamageEvent(ActualDamage, HitResult, (HitResult.TraceEnd - HitResult.TraceStart).GetSafeNormal(), DamageType);

	float ResultingDamage = HitResult.GetActor()->TakeDamage(ActualDamage, PointDamageEvent, CharacterOwner->GetController(), CharacterOwner);
}

void AZHitScanWeapon::Server_Unreliable_TraceNoHit_Implementation(const FVector& EndLocation)
{
	Multicast_Unreliable_PlayTraceEffects(EndLocation);
}

void AZHitScanWeapon::Server_Reliable_TraceHit_Implementation(const FHitResult& HitResult)
{
	TraceHit(HitResult);
	Multicast_Unreliable_PlayTraceEffects(HitResult.Location);
}

void AZHitScanWeapon::Multicast_Unreliable_PlayTraceEffects_Implementation(const FVector& HitLocation)
{
	if (CharacterOwner->IsLocallyControlled())
	{
		return;
	}
	PlayTraceEffects(HitLocation);
}

float AZHitScanWeapon::RecalculateSpread() const
{
	UZFireMode* FireMode = Cast<UZFireMode>(Actions[EZItemInput::Primary]);
	if (!ensure(FireMode))
	{
		return 0.f;
	}
	float FireModeSpread = FireMode->GetSpread();

	UZADSAction* ADSAction = Cast<UZADSAction>(Actions[EZItemInput::Secondary]);
	if (ADSAction && ADSAction->IsHeldDown())
	{
		FireModeSpread = 0.f;
	}
	return FireModeSpread * 0.01f;
}

FVector AZHitScanWeapon::GetTraceDirection(float Spread, const FRotator& ViewDirection) const
{
	if (Spread <= 0.f)
	{
		return ViewDirection.Vector();
	}
	const FVector2D Point2 = FMath::RandPointInCircle(Spread);
	const FVector Point(0.f, Point2.X, Point2.Y);
	const FVector Offset = ViewDirection.RotateVector(Point);
	const FVector TraceDirection = ViewDirection.Vector() + Offset;

	return TraceDirection.GetSafeNormal();
}
