// Copyright 2026 Luka Markuš All rights reserved.


#include "Characters/ZFPCharacter.h"
#include "ZLog.h"
#include "InputTriggers.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "ZConstants.h"
#include "ZInteractable.h"
#include "ZPlayerController.h"
#include "Inventory/ZInventoryComponent.h"
#include "Movement/ZCharacterMovementComponent.h"
#include "Components/ZEquipmentComponent.h"
#include "Player/ZPlayerState.h"
#include "Components/ZSkillsComponent.h"
#include "ZTypes.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "ZNetworkTypes.h"
#include "Characters/ZNPCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "DefaultMovementSet/CharacterMoverComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "MoveLibrary/BasedMovementUtils.h"

void FZInteractionStateEntry::PostReplicatedAdd(const FZInteractionStateArray& InArraySerializer)
{
	if (AZFPCharacter* Character = InArraySerializer.CharacterOwner.Get())
	{
		Character->OnInteractionActivityStarted.Broadcast(InteractionState);
	}
}

void FZInteractionStateEntry::PostReplicatedChange(const FZInteractionStateArray& InArraySerializer)
{
	if (AZFPCharacter* Character = InArraySerializer.CharacterOwner.Get())
	{
		Character->OnInteractionActivityUpdated.Broadcast(InteractionState);
	}
}

void FZInteractionStateEntry::PreReplicatedRemove(const FZInteractionStateArray& InArraySerializer)
{
	if (AZFPCharacter* Character = InArraySerializer.CharacterOwner.Get())
	{
		Character->OnActivityStopped.Broadcast(InteractionState.Activity);
	}
}

AZFPCharacter::AZFPCharacter()
{
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(GetMesh());
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	// FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	// FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;
	
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	FirstPersonMesh->SetupAttachment(FirstPersonCameraComponent);
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));
	FirstPersonMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;

	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::None;
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
	
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(GetCapsuleComponent());
	SpringArmComponent->TargetArmLength = 300.f;
	SpringArmComponent->bUsePawnControlRotation = true; // rotates with camera
	SpringArmComponent->bEnableCameraLag = true;
	SpringArmComponent->CameraLagSpeed = 10.f;

	ThirdPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	ThirdPersonCameraComponent->bUsePawnControlRotation = false;

	bCanRagdoll = false;

	FOVSensAdjustment = 1.f;
	FPPInteractionTraceDistance = 350.f;
	TPPInteractionTraceDistance = 600.f;

	PrimaryActorTick.bCanEverTick = true;

	Faction = EZFaction::Player;
	
	bFirstPerson = false;
	
	bMaintainLastInputOrientation = false;
	
	LastAffirmativeMoveInput = FVector::ZeroVector;
	CachedMoveInputIntent = FVector::ZeroVector;
	CachedMoveInputVelocity = FVector::ZeroVector;
	
	CachedLookInput = FRotator::ZeroRotator;
	
	VerticalSensitivity = 10.f;
	HorizontalSensitivity = 10.f;
	
	VerticalMouseSensitivity = 10.f;
	HorizontalMouseSensitivity = 10.f;
	
	RightShoulderOffset = 60.f;
	LeftShoulderOffset = -60.f;
}

bool AZFPCharacter::CanInteract(const AZCharacter* InCharacter) const
{
	if (Cast<AZFPCharacter>(InCharacter)) // Don't allow players to interact with each other
	{
		return false;
	}
	return Super::CanInteract(InCharacter);
}

bool AZFPCharacter::BeginInteract(AZCharacter* InCharacter)
{
	// PendingInteractionTarget = nullptr;
	return Super::BeginInteract(InCharacter);
}

EZCharacterActivity AZFPCharacter::GetCharacterActivity() const
{
	return bDead ? EZCharacterActivity::Looting : EZCharacterActivity::Talking;
}

void AZFPCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocallyControlled())
	{
		// Spin camera based on input
		if (AZPlayerController* PC = GetController<AZPlayerController>())
		{
			AddControllerYawInput(CachedLookInput.Yaw * HorizontalSensitivity * FOVSensAdjustment);
			AddControllerPitchInput(CachedLookInput.Pitch * VerticalSensitivity * FOVSensAdjustment);
		}

		// Clear all camera-related cached input
		CachedLookInput = FRotator::ZeroRotator;
		
		UCameraComponent* ActiveCamera = GetActiveCameraComponent();
		const FVector Start = ActiveCamera->GetComponentLocation();
		const FVector End = Start + ActiveCamera->GetForwardVector() * GetInteractionTraceDistance();

		TScriptInterface<IZInteractable> NewInteractionTarget = nullptr;
		FHitResult OutHit;
		FCollisionQueryParams CQP(NAME_None, false, this);
		if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Interactable, CQP))
		{
			if (OutHit.GetActor() && OutHit.GetActor()->Implements<UZInteractable>())
			{
				IZInteractable* Interactable = Cast<IZInteractable>(OutHit.GetActor());
				if (Interactable->ShouldHighlight(this))
				{
					NewInteractionTarget = TScriptInterface<IZInteractable>(OutHit.GetActor());
				}
			}
		}
		if (NewInteractionTarget != LookAtInteractionTarget)
		{
			if (LookAtInteractionTarget)
			{
				for (UMeshComponent* HighlightMesh : LookAtInteractionTarget->GetHighlightMeshes())
				{
					HighlightMesh->SetRenderCustomDepth(false);
					HighlightMesh->SetCustomDepthStencilValue(0);
				}
			}
			LookAtInteractionTarget = NewInteractionTarget;
			if (NewInteractionTarget)
			{
				for (UMeshComponent* HighlightMesh : LookAtInteractionTarget->GetHighlightMeshes())
				{
					HighlightMesh->SetRenderCustomDepth(true);
					HighlightMesh->SetCustomDepthStencilValue(2);
				}
			}
		}
	}
}

USkeletalMeshComponent* AZFPCharacter::GetActualMesh() const
{
	return (IsLocallyControlled() && bFirstPerson) ? FirstPersonMesh.Get() : GetMesh();
}

void AZFPCharacter::SetFOVSensAdjustment(float Val)
{
	FOVSensAdjustment = Val;
}

void AZFPCharacter::OnCrouchUpdate(float RelativeLocZ)
{
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		FVector CameraRelLoc = FirstPersonCameraComponent->GetRelativeLocation();
		CameraRelLoc.Z = RelativeLocZ;
		FirstPersonCameraComponent->SetRelativeLocation(CameraRelLoc);
	}
	else
	{
		//BaseTranslationOffset.Z = RelativeLocZ;
	}
}

bool AZFPCharacter::HasSkill(EZSkill Skill) const
{
	if (const AZPlayerState* PS = GetPlayerState<AZPlayerState>())
	{
		return PS->GetSkillsComponent()->HasSkill(Skill);
	}
	return false;
}

void AZFPCharacter::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	OutLocation = FirstPersonCameraComponent->GetComponentLocation();
	OutRotation = FirstPersonCameraComponent->GetComponentRotation();
}

void AZFPCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams ParamsOwnerOnly;
	ParamsOwnerOnly.bIsPushBased = true;
	ParamsOwnerOnly.Condition = COND_OwnerOnly;
	ParamsOwnerOnly.RepNotifyCondition = REPNOTIFY_Always;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, InteractionStates, ParamsOwnerOnly);
}

void AZFPCharacter::StartInteractionActivity(const FZInteractionState& InInteractionState)
{
	Super::StartInteractionActivity(InInteractionState);

	AddInteractionState(InInteractionState);
	// #ZTODONOW first person animation montage, also handle it in OnRep
}

void AZFPCharacter::UpdateActivity(const FZInteractionState& InInteractionState)
{
	Super::UpdateActivity(InInteractionState);

	UpdateInteractionState(InInteractionState);
}

void AZFPCharacter::StopActivity(EZCharacterActivity Activity)
{
	Super::StopActivity(Activity);

	RemoveInteractionState(Activity);
}

void AZFPCharacter::AddInteractionState(const FZInteractionState& InteractionState)
{
	FZInteractionStateEntry& NewEntry = InteractionStates.InteractionStates.AddDefaulted_GetRef();
	NewEntry.InteractionState = InteractionState;
	InteractionStates.MarkItemDirty(NewEntry);
}

void AZFPCharacter::UpdateInteractionState(const FZInteractionState& InteractionState)
{
	if (FZInteractionStateEntry* Entry = InteractionStates.InteractionStates.FindByPredicate([&InteractionState](const FZInteractionStateEntry& Entry)
		{
			return Entry.InteractionState.Activity == InteractionState.Activity;
		}))
	{
		Entry->InteractionState = InteractionState;
		InteractionStates.MarkItemDirty(*Entry);
	}
	else
	{
		ensure(0);
	}
}

void AZFPCharacter::RemoveInteractionState(EZCharacterActivity Activity)
{
	InteractionStates.InteractionStates.RemoveAll([Activity](const FZInteractionStateEntry& Entry)
	{
		return Entry.InteractionState.Activity == Activity;
	});
	InteractionStates.MarkArrayDirty();
}

void AZFPCharacter::UpdateGaitState()
{
	if (bWantsToWalk)
	{
		DesiredGaitState = EZGaitState::Walk;
	}
	else if (bWantsToSprint)
	{
		DesiredGaitState = EZGaitState::Sprint;
	}
	else
	{
		DesiredGaitState = EZGaitState::Run;
	}
	Super::UpdateGaitState();
}

void AZFPCharacter::SwitchShoulder()
{
	if (bFirstPerson)
	{
		return;
	}
	bIsRightShoulder = !bIsRightShoulder;
	OnSwitchShoulder();
}

void AZFPCharacter::OnSwitchShoulder() const
{
	const float TargetOffset = bIsRightShoulder ? RightShoulderOffset : LeftShoulderOffset;
	SpringArmComponent->SocketOffset.Y = TargetOffset;
}

UCameraComponent* AZFPCharacter::GetActiveCameraComponent() const
{
	return bFirstPerson ? FirstPersonCameraComponent : ThirdPersonCameraComponent;
}

float AZFPCharacter::GetInteractionTraceDistance() const
{
	return bFirstPerson ? FPPInteractionTraceDistance : TPPInteractionTraceDistance;
}

void AZFPCharacter::SetADSing(bool bVal)
{
	Super::SetADSing(bVal);
	
	if (!HasAuthority())
	{
		Server_Reliable_SetADSing(bVal);
	}
}

void AZFPCharacter::Server_Reliable_SetADSing_Implementation(bool bVal)
{
	SetADSing(bVal);
}

bool AZFPCharacter::StartInteractingWith(TScriptInterface<IZInteractable> InInteractionTarget)
{
	if (bDead)
	{
		return false;
	}
	if (!HasAuthority())
	{
		if (PendingInteractionTarget)
		{
			return false;
		}
		if (!InInteractionTarget->CanInteract(this))
		{
			if (AZNPCharacter* TargetNPC = Cast<AZNPCharacter>(InInteractionTarget.GetObject()))
			{
				Server_Reliable_OnInteractionAttempted(TargetNPC);
			}
			return false;
		}
		AZCharacter* InteractionTargetCharacter = Cast<AZCharacter>(InInteractionTarget.GetObject());
		if (InteractionTargetCharacter && !CanInteract(InteractionTargetCharacter))
		{
			return false;
		}
		PendingInteractionTarget = InInteractionTarget;
		Server_Reliable_StartInteractingWith(Cast<AActor>(InInteractionTarget.GetObject()));
		return true;
	}
	return Super::StartInteractingWith(InInteractionTarget);
}

void AZFPCharacter::StopInteractingWith(TScriptInterface<IZInteractable> InInteractionTarget, bool bForceInstant)
{
	if (!HasAuthority())
	{
		PendingInteractionTarget = nullptr;
		Server_Reliable_StopInteractingWith(Cast<AActor>(InInteractionTarget.GetObject()));
		return;
	}
	Super::StopInteractingWith(InInteractionTarget, bForceInstant);
}

FGenericTeamId AZFPCharacter::GetGenericTeamId() const
{
	if (const AZPlayerController* PC = GetController<AZPlayerController>())
	{
		return PC->GetGenericTeamId();
	}
	else
	{
		return FGenericTeamId::NoTeam;
	}
}

void AZFPCharacter::BeginPlay()
{
	Super::BeginPlay();

	InteractionStates.CharacterOwner = this;
	if (IsLocallyControlled())
	{
		OnPerspectiveChanged();
		OnSwitchShoulder();
		GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
	}
	else
	{
		FirstPersonMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
		FirstPersonMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
		GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
		
		PrimaryActorTick.SetTickFunctionEnable(false);
	}
}

void AZFPCharacter::ProduceInput_Implementation(int32 SimTimeMs, FMoverInputCmdContext& InputCmdResult)
{
	FCharacterDefaultInputs& CharacterInputs = InputCmdResult.InputCollection.FindOrAddMutableDataByType<FCharacterDefaultInputs>();
	const APlayerController* PC = Cast<APlayerController>(GetController());
	if (!bMovementEnabled || !PC)
	{
		static const FCharacterDefaultInputs DoNothingInput;
		CharacterInputs = DoNothingInput;
		return;
	}

	CharacterInputs.ControlRotation = PC->GetControlRotation();
	
	// Favor velocity input 
	const bool bUsingInputIntentForMove = CachedMoveInputVelocity.IsZero();
	if (bUsingInputIntentForMove)
	{
		const FRotator MoveRotation = FRotator(0.f, CharacterInputs.ControlRotation.Yaw, 0.f);
		const FVector FinalDirectionalIntent = MoveRotation.RotateVector(CachedMoveInputIntent);
		CharacterInputs.SetMoveInput(EMoveInputType::DirectionalIntent, FinalDirectionalIntent.GetSafeNormal());
	}
	else
	{
		CharacterInputs.SetMoveInput(EMoveInputType::Velocity, CachedMoveInputVelocity);
	}
	
	static float RotationMagMin(1e-3);

	const bool bHasAffirmativeMoveInput = (CharacterInputs.GetMoveInput().Size() >= RotationMagMin);
	
	// Figure out intended orientation
	CharacterInputs.OrientationIntent = FVector::ZeroVector;

	if (bHasAffirmativeMoveInput)
	{
		if (bOrientRotationToMovement)
		{
			// set the intent to the actors movement direction
			CharacterInputs.OrientationIntent = CharacterInputs.GetMoveInput().GetSafeNormal();
		}
		else
		{
			// set intent to the the control rotation - often a player's camera rotation
			CharacterInputs.OrientationIntent = CharacterInputs.ControlRotation.Vector().GetSafeNormal();
		}

		LastAffirmativeMoveInput = CharacterInputs.GetMoveInput();
	}
	else if (bMaintainLastInputOrientation)
	{
		// There is no movement intent, so use the last-known affirmative move input
		CharacterInputs.OrientationIntent = LastAffirmativeMoveInput;
	}
	
	CharacterInputs.bIsJumpPressed = bIsJumpPressed;
	CharacterInputs.bIsJumpJustPressed = bIsJumpJustPressed;

	CharacterInputs.SuggestedMovementMode = NAME_None;

	// Convert inputs to be relative to the current movement base (depending on options and state)
	CharacterInputs.bUsingMovementBase = false;

	if (bUseBaseRelativeMovement)
	{
		if (const UCharacterMoverComponent* MoverComp = GetComponentByClass<UCharacterMoverComponent>())
		{
			if (UPrimitiveComponent* MovementBase = MoverComp->GetMovementBase())
			{
				const FName MovementBaseBoneName = MoverComp->GetMovementBaseBoneName();

				FVector RelativeMoveInput, RelativeOrientDir;

				UBasedMovementUtils::TransformWorldDirectionToBased(MovementBase, MovementBaseBoneName, CharacterInputs.GetMoveInput(), RelativeMoveInput);
				UBasedMovementUtils::TransformWorldDirectionToBased(MovementBase, MovementBaseBoneName, CharacterInputs.OrientationIntent, RelativeOrientDir);

				CharacterInputs.SetMoveInput(CharacterInputs.GetMoveInputType(), RelativeMoveInput);
				CharacterInputs.OrientationIntent = RelativeOrientDir;

				CharacterInputs.bUsingMovementBase = true;
				CharacterInputs.MovementBase = MovementBase;
				CharacterInputs.MovementBaseBoneName = MovementBaseBoneName;
			}
		}
	}
 
	// Clear/consume temporal movement inputs. We are not consuming others in the event that the game world is ticking at a lower rate than the Mover simulation. 
	// In that case, we want most input to carry over between simulation frames.
	{
		bIsJumpJustPressed = false;
	}
}

void AZFPCharacter::MoveInput(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	CachedMoveInputIntent.X = FMath::Clamp(MovementVector.X, -1.0f, 1.0f);
	CachedMoveInputIntent.Y = FMath::Clamp(MovementVector.Y, -1.0f, 1.0f);
}

void AZFPCharacter::MoveInputCompleted(const FInputActionValue& Value)
{
	CachedMoveInputIntent = FVector::ZeroVector;
}

void AZFPCharacter::LookInput(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	CachedLookInput.Yaw = LookAxisVector.X;
	CachedLookInput.Pitch = LookAxisVector.Y;
}

void AZFPCharacter::LookInputCompleted(const FInputActionValue& Value)
{
	CachedLookInput = FRotator::ZeroRotator;
}

void AZFPCharacter::MouseLookInput(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	AddControllerYawInput(LookAxisVector.X * HorizontalMouseSensitivity * FOVSensAdjustment);
	AddControllerPitchInput(LookAxisVector.Y * VerticalMouseSensitivity * FOVSensAdjustment);
}

void AZFPCharacter::DoInteract()
{
	if (!LookAtInteractionTarget)
	{
		return;
	}
	if (GetInteractionTarget(LookAtInteractionTarget->GetCharacterActivity())) // Already interacting with something with that activity
	{
		return;
	}
	StartInteractingWith(LookAtInteractionTarget);
}

void AZFPCharacter::DoSprintStart()
{
	bWantsToSprint = true;
	UpdateGaitState();
}

void AZFPCharacter::DoSprintEnd()
{
	bWantsToSprint = false;
	UpdateGaitState();
}

void AZFPCharacter::WalkStart()
{
	bWantsToWalk = true;
	UpdateGaitState();
}

void AZFPCharacter::WalkEnd()
{
	bWantsToWalk = false;
	UpdateGaitState();
}

void AZFPCharacter::DoOpenInventory()
{
	if (AZPlayerController* PC = GetController<AZPlayerController>())
	{
		PC->ToggleInventory();
	}
}

void AZFPCharacter::DoOpenQuests()
{
	if (AZPlayerController* PC = GetController<AZPlayerController>())
	{
		PC->ToggleQuests();
	}
}

void AZFPCharacter::DoOpenSkills()
{
	if (AZPlayerController* PC = GetController<AZPlayerController>())
	{
		PC->ToggleSkills();
	}
}

void AZFPCharacter::DoOpenMap()
{
	if (AZPlayerController* PC = GetController<AZPlayerController>())
	{
		PC->ToggleMap();
	}
}

void AZFPCharacter::Escape()
{
	if (AZPlayerController* PC = GetController<AZPlayerController>())
	{
		PC->Escape();
	}
}

void AZFPCharacter::ChangePerspective()
{
	bFirstPerson = !bFirstPerson;
	OnPerspectiveChanged();
}

void AZFPCharacter::OnPerspectiveChanged()
{
	FirstPersonCameraComponent->SetActive(bFirstPerson);
	ThirdPersonCameraComponent->SetActive(!bFirstPerson);
	FirstPersonMesh->SetOwnerNoSee(!bFirstPerson);
	FirstPersonMesh->SetAnimationMode(bFirstPerson ? EAnimationMode::AnimationBlueprint : EAnimationMode::AnimationSingleNode);
	FirstPersonMesh->VisibilityBasedAnimTickOption = bFirstPerson ? EVisibilityBasedAnimTickOption::AlwaysTickPose : EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	GetMesh()->SetOwnerNoSee(bFirstPerson);
	GetMesh()->SetAnimationMode(bFirstPerson ? EAnimationMode::AnimationSingleNode : EAnimationMode::AnimationBlueprint);
	GetMesh()->VisibilityBasedAnimTickOption = bFirstPerson ? EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered : EVisibilityBasedAnimTickOption::AlwaysTickPose;
	// reequip equipped item
	// reattach props
}

void AZFPCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ThisClass::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ThisClass::JumpEnd);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::MoveInput);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ThisClass::MoveInputCompleted);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::LookInput);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Completed, this, &ThisClass::LookInputCompleted);
		
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ThisClass::MouseLookInput);
		
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ThisClass::DoInteract);

		EnhancedInputComponent->BindAction(InventoryAction, ETriggerEvent::Started, this, &ThisClass::DoOpenInventory);
		
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ThisClass::DoSprintStart);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ThisClass::DoSprintEnd);
		
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ThisClass::CrouchStart);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ThisClass::CrouchEnd);
		
		EnhancedInputComponent->BindAction(WalkAction, ETriggerEvent::Started, this, &ThisClass::WalkStart);
		EnhancedInputComponent->BindAction(WalkAction, ETriggerEvent::Completed, this, &ThisClass::WalkEnd);

		EnhancedInputComponent->BindAction(UnequipAction, ETriggerEvent::Started, this, &ThisClass::DoUnequip);

		EnhancedInputComponent->BindAction(OpenQuestsAction, ETriggerEvent::Started, this, &ThisClass::DoOpenQuests);

		EnhancedInputComponent->BindAction(OpenSkillsAction, ETriggerEvent::Started, this, &ThisClass::DoOpenSkills);

		EnhancedInputComponent->BindAction(OpenMapAction, ETriggerEvent::Started, this, &ThisClass::DoOpenMap);

		EnhancedInputComponent->BindAction(EscapeAction, ETriggerEvent::Started, this, &ThisClass::Escape);
		
		EnhancedInputComponent->BindAction(ChangePerspectiveAction, ETriggerEvent::Started, this, &ThisClass::ChangePerspective);
		
		EnhancedInputComponent->BindAction(SlideAction, ETriggerEvent::Started, this, &ThisClass::Slide);
		EnhancedInputComponent->BindAction(SlideAction, ETriggerEvent::Completed, this, &ThisClass::SlideEnd);

		EnhancedInputComponent->BindAction(SwitchShoulderAction, ETriggerEvent::Started, this, &ThisClass::SwitchShoulder);
		
		GetEquipmentComponent()->SetupPlayerInputComponent(EnhancedInputComponent);
	}
	else
	{
		UE_LOG(LogZombies, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AZFPCharacter::Die(AController* DeathInstigator)
{
	Super::Die(DeathInstigator);

	FirstPersonCameraComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform); // #ZTODO switch to third person?
}

void AZFPCharacter::Server_Reliable_StartInteractingWith_Implementation(AActor* InInteractableActor)
{
	if (!StartInteractingWith(TScriptInterface<IZInteractable>(InInteractableActor)))
	{
		Client_Reliable_StartInteractingWithFailed();
	}
}

void AZFPCharacter::Client_Reliable_StartInteractingWithFailed_Implementation()
{
	PendingInteractionTarget = nullptr;
}

void AZFPCharacter::Server_Reliable_StopInteractingWith_Implementation(AActor* InInteractableActor)
{
	StopInteractingWith(TScriptInterface<IZInteractable>(InInteractableActor));
}

void AZFPCharacter::Server_Reliable_OnInteractionAttempted_Implementation(AZNPCharacter* TargetNPC)
{
	if (ensure(TargetNPC))
	{
		TargetNPC->OnInteractionAttempted(this);
	}
}

