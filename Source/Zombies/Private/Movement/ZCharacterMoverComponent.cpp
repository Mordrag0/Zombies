// Copyright 2026 Luka Markuš. All rights reserved.


#include "Movement/ZCharacterMoverComponent.h"
#include "ChaosMover/Character/ChaosCharacterMoverComponent.h"
#include "DefaultMovementSet/InstantMovementEffects/BasicInstantMovementEffects.h"
#include "Physics/Experimental/PhysScene_Chaos.h"

DEFINE_LOG_CATEGORY(LogZCharacterMover)

void UZCharacterMoverComponent::BeginPlay()
{	Super::BeginPlay();
	
#if WITH_EDITOR
	if (IsA<UChaosCharacterMoverComponent>())
	{
		if (UpdatedComponent)
		{
			if (const UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(UpdatedComponent))
			{
				if (Primitive->GetBodyInstance())
				{
					ensureMsgf(Primitive->GetBodyInstance()->bUpdateKinematicFromSimulation, 
						TEXT("UpdateKinematicFromSimulation must be enabled on %s's updated component for Chaos Mover to work correctly"), *GetOwner()->GetName());
					ensureMsgf(Primitive->GetBodyInstance()->bSimulatePhysics, 
						TEXT("bSimulatePhysics must be enabled on %s's updated component for Chaos Mover to work correctly"), *GetOwner()->GetName());
				}
				else
				{
					UE_LOG(LogZCharacterMover, Warning, TEXT("Updated component body instance is null: %s"), *GetOwner()->GetName());
				}
			}
			else
			{
				UE_LOG(LogZCharacterMover, Warning, TEXT("Updated component is not primitive component: %s"), *GetOwner()->GetName());
			}
		}
		else
		{
			UE_LOG(LogZCharacterMover, Warning, TEXT("Missing updated component: %s"), *GetOwner()->GetName());
		}
	}
#endif
}

void UZCharacterMoverComponent::Teleport(const FVector& Location)
{
	TSharedPtr<FTeleportEffect> TeleportEffect = MakeShared<FTeleportEffect>();
	TeleportEffect->TargetLocation = Location;
	TeleportEffect->bUseActorRotation = true;
	QueueInstantMovementEffect(TeleportEffect);
}

void UZCharacterMoverComponent::Teleport(const FVector& Location, const FRotator& Rotation)
{
	TSharedPtr<FTeleportEffect> TeleportEffect = MakeShared<FTeleportEffect>();
	TeleportEffect->TargetLocation = Location;
	TeleportEffect->bUseActorRotation = false;
	TeleportEffect->TargetRotation = Rotation;
	QueueInstantMovementEffect(TeleportEffect);
}
