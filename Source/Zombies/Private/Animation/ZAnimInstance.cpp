// Copyright 2026 Luka Markuš. All rights reserved.


#include "Animation/ZAnimInstance.h"
#include "Characters/ZCharacter.h"
#include "ZTypes.h"
#include "Movement/ZCharacterMoverComponent.h"

UZAnimInstance::UZAnimInstance()
{
	RagdollPoseDuration = .3f;
	RagdollPoseName = FName("RagdollPose");
}

void UZAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	CharacterOwner = Cast<AZCharacter>(TryGetPawnOwner());
	if (CharacterOwner)
	{
		CharacterMover = CharacterOwner->GetMoverComponent();
	}
}

void UZAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (!CharacterOwner || !CharacterMover)
	{
		return;
	}
	bDead = CharacterOwner->GetDead();
	bMirror = CharacterOwner->GetMirrorAnimation();
	CurrentActivities = CharacterOwner->GetCurrentActivities();

	if (CharacterOwner->GetLookAtTarget())
	{
		const FVector Direction = CharacterOwner->GetLookAtTarget()->GetActorLocation() - CharacterOwner->GetActorLocation();
		const FRotator Rotation = CharacterOwner->GetActorRotation().UnrotateVector(Direction).Rotation();
		LookUp = Rotation.Pitch;
		LookRight = Rotation.Yaw;
	}
	else
	{
		LookUp = 0.f;
		LookRight = 0.f;
	}
	
	const EZGaitState GaitState = EZGaitState::Run;// CharacterMovement->GetGaitState(); #ZTODOMOVER
	const FVector Velocity = CharacterOwner->GetVelocity();
	const FVector LocalVelocity = CharacterOwner->GetActorRotation().UnrotateVector(Velocity);
	const float MaxSpeed = 600.f; // CharacterMovement->GetMaxSpeed(); #ZTODOMOVER
	if (MaxSpeed > 0)
	{
		WalkForward = LocalVelocity.X / MaxSpeed;
		WalkRight = LocalVelocity.Y / MaxSpeed;
	}
	else
	{
		WalkForward = 0.f;
		WalkRight = 0.f;
	}
	if (GaitState == EZGaitState::Walk)
	{
		WalkForward *= .5f;
		WalkRight *= .5f;
	}
	else if (GaitState == EZGaitState::Sprint)
	{
		WalkForward *= 2.f;
	}
}

void UZAnimInstance::SaveRagdollPoseSnapShot()
{
	SavePoseSnapshot(RagdollPoseName);
}

void UZAnimInstance::OnExitRagdoll(const FTransform& InTransform)
{
	SnapshotRelativeTransform = InTransform;
	bRagdollPose = true;
}

void UZAnimInstance::OnEndRagdollSnapShot()
{
	bRagdollPose = false;
}

bool UZAnimInstance::IsPerformingActivityBP(EZCharacterActivityBP Activity) const
{
	return IsPerformingActivity(ToCharacterActivity(Activity));
}


