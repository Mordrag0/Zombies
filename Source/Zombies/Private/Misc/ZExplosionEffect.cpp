// Copyright 2026 Luka Markuš. All rights reserved.


#include "Misc/ZExplosionEffect.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SceneComponent.h"
#include "Components/PointLightComponent.h"
#include "Sound/SoundCue.h"

AZExplosionEffect::AZExplosionEffect()
{
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;

	PointLightComponent = CreateDefaultSubobject<UPointLightComponent>("Point Light Component");
	PointLightComponent->SetupAttachment(Root);
	PointLightComponent->AttenuationRadius = 400.0;
	PointLightComponent->bUseInverseSquaredFalloff = false;
	PointLightComponent->LightColor = FColor(255, 185, 35);

	PrimaryActorTick.bCanEverTick = true;

	LightIntensity = 500.f;
	ExplosionLightFadeOut = .2f;
}

void AZExplosionEffect::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const float TimeAlive = GetWorld()->GetTimeSeconds() - CreationTime;
	const float TimeRemaining = FMath::Max(0.0f, ExplosionLightFadeOut - TimeAlive);

	if (TimeRemaining > 0.f)
	{
		const float FadeAlpha = 1.0f - FMath::Square(TimeRemaining / ExplosionLightFadeOut);
		PointLightComponent->SetIntensity(LightIntensity * FadeAlpha);
	}
	else
	{
		Destroy();
	}
}

void AZExplosionEffect::BeginPlay()
{
	Super::BeginPlay();
	
	if (VFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), VFX, GetActorTransform());
	}
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation(), 1.f);
	}
}



