// Copyright 2026 Luka Markuš. All rights reserved.


#include "Movement/ZCharacterMovementComponent.h"
#include "Characters/ZCharacter.h"
#include "Components/ZStaminaComponent.h"
#include "Components/CapsuleComponent.h"
#include "Characters/ZFPCharacter.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"


FZCharacterNetworkMoveData::FZCharacterNetworkMoveData()
{
    GaitState = EZGaitState::None;
    CrouchAmount = 0.f;
}

FZCharacterNetworkMoveDataContainer::FZCharacterNetworkMoveDataContainer()
{
    NewMoveData = &MoveData[0];
    PendingMoveData = &MoveData[1];
    OldMoveData = &MoveData[2];
}

void FZCharacterNetworkMoveData::ClientFillNetworkMoveData(const FSavedMove_Character& ClientMove, ENetworkMoveType MoveType)
{
    Super::ClientFillNetworkMoveData(ClientMove, MoveType);
    const FSavedMove_ZCharacter& ZMove = static_cast<const FSavedMove_ZCharacter&>(ClientMove);
    GaitState = ZMove.SavedGaitState;
    CrouchAmount = ZMove.CrouchAmount;
}

bool FZCharacterNetworkMoveData::Serialize(UCharacterMovementComponent& CharacterMovement, FArchive& Ar, UPackageMap* PackageMap, ENetworkMoveType MoveType)
{
    Super::Serialize(CharacterMovement, Ar, PackageMap, MoveType);
    Ar << GaitState;
    Ar << CrouchAmount;
    return !Ar.IsError();
}

void FSavedMove_ZCharacter::Clear()
{
	Super::Clear();

	// Clear variables back to their default states
	SavedGaitState = EZGaitState::None;
	CrouchAmount = 0.f;
}

bool FSavedMove_ZCharacter::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const
{
	const FSavedMove_ZCharacter* NewZMove = static_cast<const FSavedMove_ZCharacter*>(NewMove.Get());
	if (SavedGaitState != NewZMove->SavedGaitState)
	{
		return false;
	}
	if (bWantsToCrouch != NewZMove->bWantsToCrouch)
	{
		return false;
	}
	if (FMath::IsNearlyZero(CrouchAmount) != FMath::IsNearlyZero(NewZMove->CrouchAmount))
	{
		return false;
	}
	if (FMath::IsNearlyZero(CrouchAmount - 1) != FMath::IsNearlyZero(NewZMove->CrouchAmount - 1))
	{
		return false;
	}

	return Super::CanCombineWith(NewMove, Character, MaxDelta);
}

void FSavedMove_ZCharacter::CombineWith(const FSavedMove_Character* OldMove, ACharacter* InCharacter, APlayerController* PC, const FVector& OldStartLocation)
{
	Super::CombineWith(OldMove, InCharacter, PC, OldStartLocation);

	const FSavedMove_ZCharacter* OldEXMove = static_cast<const FSavedMove_ZCharacter*>(OldMove);


}

void FSavedMove_ZCharacter::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData)
{
	AZCharacter* ZCharacter = nullptr; // Cast<AZCharacter>(Character);
	UZCharacterMovementComponent* ZCharacterMovement = nullptr;
	if (ZCharacter && ZCharacterMovement)
	{
		SavedGaitState = ZCharacterMovement->GaitState;
		CrouchAmount = ZCharacterMovement->GetCrouchAmount();
	}
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);
}

void FSavedMove_ZCharacter::PrepMoveFor(class ACharacter* Character)
{
	Super::PrepMoveFor(Character);

  //   if (UZCharacterMovementComponent* MoveComp = Cast<UZCharacterMovementComponent>(nullptr))
  //   {
		// MoveComp->GaitState = SavedGaitState;
		// MoveComp->CrouchAmount = CrouchAmount;
  //   }
}

void FSavedMove_ZCharacter::SetInitialPosition(ACharacter* Character)
{
	Super::SetInitialPosition(Character);

	AZCharacter* ZCharacter = nullptr; // Cast<AZCharacter>(Character);
	UZCharacterMovementComponent* ZCharacterMovement = nullptr;
	if (ZCharacter && ZCharacterMovement)
	{
	}
}

void FSavedMove_ZCharacter::PostUpdate(ACharacter* Character, EPostUpdateMode PostUpdateMode)
{
	Super::PostUpdate(Character, PostUpdateMode);
	
	AZCharacter* ZCharacter = nullptr; // Cast<AZCharacter>(Character);
	UZCharacterMovementComponent* ZCharacterMovement = nullptr;
	if (ZCharacter && ZCharacterMovement)
	{
		//UCapsuleComponent* Capsule = ZCharacter->GetCapsuleComponent();
		//SavedLocation.Z -= Capsule->GetScaledCapsuleHalfHeight();
	}
}

FNetworkPredictionData_Client_ZCharacter::FNetworkPredictionData_Client_ZCharacter(const UCharacterMovementComponent& ClientMovement)
	: FNetworkPredictionData_Client_Character(ClientMovement)
{
}


UZCharacterMovementComponent::UZCharacterMovementComponent()
{
	CrouchAmount = 0.f;

	CrouchTime = .25f;
	CrouchJumpTime = .25f;

	MaxRunSpeed = 600.f;
	MaxWalkSpeed = 250.f;

	SetNetworkMoveDataContainer(MoveDataContainer);
	SetIsReplicatedByDefault(true);
}

//void UZCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
//{
//	if (!ZCharacterOwner)
//	{
//		return;
//	}
//
//	// The Flags parameter contains the compressed input flags that are stored in the saved move.
//	// UpdateFromCompressed flags simply copies the flags from the saved move into the movement component.
//	// It basically just resets the movement component to the state when the move was made so it can simulate from there.
//
//	const bool bWasPressingJump = ZCharacterOwner->bPressedJump;
//	CharacterOwner->bPressedJump = ((Flags & FSavedMove_Character::FLAG_JumpPressed) != 0);
//	bWantsToCrouch = ((Flags & FSavedMove_Character::FLAG_WantsToCrouch) != 0);
//	if ((Flags & FLAG_SPRINT) != 0)
//	{
//		SetGaitState(EZGaitState::Sprint);
//	}
//	else if ((Flags & FLAG_WALK) != 0)
//	{
//		SetGaitState(EZGaitState::Walk);
//	}
//	else
//	{
//		SetGaitState(EZGaitState::Run);
//	}
//
//	if (ZCharacterOwner->HasAuthority())
//	{
//		// bIsCrouched is only set to call OnReps
//		ZCharacterOwner->bIsCrouched = bWantsToCrouch;
//
//		const bool bIsPressingJump = CharacterOwner->bPressedJump;
//		if (bIsPressingJump && !bWasPressingJump)
//		{
//			ZCharacterOwner->Jump();
//		}
//		else if (!bIsPressingJump)
//		{
//			ZCharacterOwner->StopJumping();
//		}
//	}
//}

class FNetworkPredictionData_Client* UZCharacterMovementComponent::GetPredictionData_Client() const
{
	if (!ClientPredictionData)
	{
		UZCharacterMovementComponent* MutableThis = const_cast<UZCharacterMovementComponent*>(this);

		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_ZCharacter(*this);
		/**
		 * Copied value from UCharacterMovementComponent::NetworkMaxSmoothUpdateDistance.
		 * Maximum distance character is allowed to lag behind server location when interpolating between updates. 
		 */
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		/**
		 * Copied value from UCharacterMovementComponent::NetworkNoSmoothUpdateDistance.
		 * Maximum distance beyond which character is teleported to the new server location without any smoothing. 
		*/
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
	}

	return ClientPredictionData;
}

void UZCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	ZCharacterOwner = nullptr; // Cast<AZCharacter>(CharacterOwner);
	StaminaComponent = ZCharacterOwner->GetStaminaComponent();

	MaxSprintSpeed = 1000.f;

	CrouchTimeInv = 1.f / CrouchTime;
	CrouchJumpTimeInv = 1.f / CrouchJumpTime;
	
	const AZCharacter* DefaultCharacter = ZCharacterOwner->GetClass()->GetDefaultObject<AZCharacter>();
	DefaultCapsuleHalfHeight = DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	DefaultMeshRelativeZ = DefaultCharacter->GetMesh()->GetRelativeLocation().Z;
	if (Cast<AZFPCharacter>(ZCharacterOwner))
	{
		const AZFPCharacter* DefaultFPCharacter = ZCharacterOwner->GetClass()->GetDefaultObject<AZFPCharacter>();
		if (DefaultFPCharacter)
		{
			DefaultCameraRelativeZ = DefaultFPCharacter->GetFirstPersonCameraComponent()->GetRelativeLocation().Z;
		}
	}
}

float UZCharacterMovementComponent::GetMaxSpeed() const
{
	switch(MovementMode)
	{
	case MOVE_Walking:
	case MOVE_NavWalking:
		if (IsCrouching())
		{
			return MaxWalkSpeedCrouched;
		}
		else if (GaitState == EZGaitState::Sprint)
		{
			return MaxSprintSpeed;
		}
		else if (GaitState == EZGaitState::Run)
		{
			return MaxRunSpeed;
		}
		else if (GaitState == EZGaitState::Walk)
		{
			return MaxWalkSpeed;
		}
		else
		{
			return 0.f;
		}
	case MOVE_Falling:
		return MaxWalkSpeed;
	case MOVE_Swimming:
		return MaxSwimSpeed;
	case MOVE_Flying:
		return MaxFlySpeed;
	case MOVE_Custom:
		return MaxCustomMovementSpeed;
	case MOVE_None:
	default:
		return 0.f;
	}
}

bool UZCharacterMovementComponent::IsMovingForward() const
{
	return (FVector::DotProduct(ZCharacterOwner->GetVelocity().GetSafeNormal2D(), ZCharacterOwner->GetActorRotation().Vector()) > 0.25f);
}

bool UZCharacterMovementComponent::IsMoving() const
{
	return (ZCharacterOwner->GetVelocity().SizeSquared() > 10.f);
}

void UZCharacterMovementComponent::UpdateCrouch(float DeltaTime)
{
	const float CrouchDelta = DeltaTime * ((MovementMode == EMovementMode::MOVE_Walking) ? CrouchTimeInv : CrouchJumpTimeInv);

	const bool bClientSimulation = (ZCharacterOwner->GetLocalRole() == ROLE_SimulatedProxy);
	if (bWantsToCrouch && (CrouchAmount < 1.f) && (bClientSimulation || CanCrouchInCurrentState()))
	{
		const float NewCrouchAmount = FMath::Min(CrouchAmount + CrouchDelta, 1.f);
		if (CrouchResize(NewCrouchAmount, NewCrouchAmount - CrouchAmount, bClientSimulation))
		{
			CrouchAmount = NewCrouchAmount;
		}
	}
	else if (!bWantsToCrouch && (CrouchAmount > 0.f))
	{
		const float NewCrouchAmount = FMath::Max(CrouchAmount - CrouchDelta, 0.f);
		if (CrouchResize(NewCrouchAmount, NewCrouchAmount - CrouchAmount, bClientSimulation))
		{
			CrouchAmount = NewCrouchAmount;
		}
	}
}

void UZCharacterMovementComponent::Crouch(bool bClientSimulation)
{
	bWantsToCrouch = true;
	if (ZCharacterOwner->HasAuthority())
    {
        // ZCharacterOwner->bIsCrouched = true;
    }
}

void UZCharacterMovementComponent::UnCrouch(bool bClientSimulation)
{
	bWantsToCrouch = false;
	if (ZCharacterOwner->HasAuthority())
    {
        // ZCharacterOwner->bIsCrouched = false;
    }
}

bool UZCharacterMovementComponent::IsCrouching() const
{
	return (CrouchAmount > 0.f);
}

void UZCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	// Super handles crouching changes but we don't need that here
}

void UZCharacterMovementComponent::UpdateCharacterStateAfterMovement(float DeltaSeconds)
{
	// Super handles crouching changes but we don't need that here
}

void UZCharacterMovementComponent::SetGaitState(EZGaitState InGaitState)
{
	if (DesiredGaitState == InGaitState)
	{
		return;
	}
	DesiredGaitState = InGaitState;
	UpdateGaitState();
}

void UZCharacterMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.Condition = COND_SimulatedOnly;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, GaitState, Params);
}

void UZCharacterMovementComponent::SimulateMovement(float DeltaTime)
{
	UpdateCrouch(DeltaTime);
	//UpdateGaitState();
	Super::SimulateMovement(DeltaTime);
}

void UZCharacterMovementComponent::PerformMovement(float DeltaTime)
{
	UpdateCrouch(DeltaTime);
	UpdateGaitState();
	Super::PerformMovement(DeltaTime);
}

bool UZCharacterMovementComponent::CrouchResize(float NewCrouchAmount, float DeltaCrouch, bool bClientSimulation)
{
	if (!HasValidData())
	{
		return false;
	}
	UCapsuleComponent* Capsule = nullptr; // CharacterOwner->GetCapsuleComponent();
	const float OldUnscaledHalfHeight = Capsule->GetUnscaledCapsuleHalfHeight();
	const float OldUnscaledRadius = Capsule->GetUnscaledCapsuleRadius();

	const float DefaultFullCrouchDiff = DefaultCapsuleHalfHeight - GetCrouchedHalfHeight();

	const float HalfHeightAdjust = DefaultFullCrouchDiff * DeltaCrouch;
	// Height is not allowed to be smaller than radius.
	Capsule->SetCapsuleSize(OldUnscaledRadius, 
							FMath::Max(OldUnscaledRadius, 
									   DefaultCapsuleHalfHeight - NewCrouchAmount * DefaultFullCrouchDiff));
	if (!bClientSimulation)
	{
		if (bCrouchMaintainsBaseLocation)
		{
			// Intentionally not using MoveUpdatedComponent, where a horizontal 
			// plane constraint would prevent the base of the capsule from staying at the same spot.
			UpdatedComponent->MoveComponent(FVector(0.f, 0.f, -HalfHeightAdjust), UpdatedComponent->GetComponentQuat(), true, nullptr, 
											EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
		}

		if (DeltaCrouch < 0) // Uncrouching
		{
			const FCollisionShape StandingCapsuleShape = GetPawnCapsuleCollisionShape(EShrinkCapsuleExtent::SHRINK_None);
			const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();
			const FVector PawnLocation = UpdatedComponent->GetComponentLocation();
			FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(CrouchTrace), false, ZCharacterOwner);
			FCollisionResponseParams ResponseParam;
			InitCollisionParams(CapsuleParams, ResponseParam);
			bool bEncroached = GetWorld()->OverlapBlockingTestByChannel(PawnLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);
			if (bEncroached)
			{
				Capsule->SetCapsuleSize(OldUnscaledRadius, OldUnscaledHalfHeight);
				if (bCrouchMaintainsBaseLocation)
				{
					UpdatedComponent->MoveComponent(FVector(0.f, 0.f, HalfHeightAdjust), UpdatedComponent->GetComponentQuat(), true, nullptr,
													EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
				}
				return false;
			}
		}
	}

	bForceNextFloorCheck = true;
	bShrinkProxyCapsule = true;
	AdjustProxyCapsuleSize();

	if (bClientSimulation)
	{
		// FVector& MeshRelativeLocation = CharacterOwner->GetMesh()->GetRelativeLocation_DirectMutable();
		// MeshRelativeLocation.Z = DefaultMeshRelativeZ + DefaultFullCrouchDiff * NewCrouchAmount;
		// ZCharacterOwner->OnCrouchUpdate(MeshRelativeLocation.Z);
	}
	else if (ZCharacterOwner->GetLocalRole() == ROLE_AutonomousProxy)
	{
		if (AZFPCharacter* FPCharacterOwner = Cast<AZFPCharacter>(ZCharacterOwner))
		{
			FVector& CameraRelativeLocation = FPCharacterOwner->GetFirstPersonCameraComponent()->GetRelativeLocation_DirectMutable();
			CameraRelativeLocation.Z = DefaultCameraRelativeZ - DefaultFullCrouchDiff * NewCrouchAmount / 2;
			ZCharacterOwner->OnCrouchUpdate(CameraRelativeLocation.Z);
		}
	}

	// Don't smooth this change in mesh position
	if (bClientSimulation && (ZCharacterOwner->GetLocalRole() == ROLE_SimulatedProxy))
	{
		FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character();
		if (ClientData && (ClientData->MeshTranslationOffset.Z != 0.f))
		{
			ClientData->MeshTranslationOffset -= FVector(0.f, 0.f, HalfHeightAdjust);
			ClientData->OriginalMeshTranslationOffset = ClientData->MeshTranslationOffset;
		}
	}
	return true;
}

void UZCharacterMovementComponent::UpdateGaitState()
{
	EZGaitState OldGaitState = GaitState;
	if (DesiredGaitState == EZGaitState::Sprint && !IsMovingForward())
	{
		GaitState = EZGaitState::Run;
	}
	else
	{
		GaitState = DesiredGaitState;
	}
	// if ((OldGaitState != GaitState) && CharacterOwner->HasAuthority())
	// {
	// 	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, GaitState, this);
	// }
}

void UZCharacterMovementComponent::MoveAutonomous(float ClientTimeStamp, float DeltaTime, uint8 CompressedFlags, const FVector& NewAccel)
{
	const FZCharacterNetworkMoveData* ZMoveData = static_cast<const FZCharacterNetworkMoveData*>(GetCurrentNetworkMoveData());
    if (ZMoveData)
    {
        DesiredGaitState = ZMoveData->GaitState;
        CrouchAmount = ZMoveData->CrouchAmount;
    }
	if (ZCharacterOwner->HasAuthority())
	{
		//ZCharacterOwner->bIsCrouched = bWantsToCrouch;
	}
	Super::MoveAutonomous(ClientTimeStamp, DeltaTime, CompressedFlags, NewAccel);
}


