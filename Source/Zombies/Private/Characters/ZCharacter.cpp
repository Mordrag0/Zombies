// Copyright 2026 Luka Markuš. All rights reserved.


#include "Characters/ZCharacter.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Inventory/ZInventoryComponent.h"
#include "Inventory/ZInventoryItem.h"
#include "Components/CapsuleComponent.h"
#include "ZConstants.h"
#include "Components/ZStaminaComponent.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Engine/DamageEvents.h"
#include "Animation/AnimInstance.h"
#include "Animation/ZAnimInstance.h"
#include "Components/ZEquipmentComponent.h"
#include "ZGameMode.h"
#include "ZGameplayStatics.h"
#include "AI/ZAIController.h"
#include "SaveLoad/ZSaveComponent.h"
#include "Misc/ZBed.h"
#include "ZTypes.h"
#include "ZNetworkTypes.h"
#include "Characters/ZNPCharacter.h"
#include "Animation/ZCharacterAnimationData.h"
#include "ChaosMover/Character/ChaosCharacterMoverComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Movement/ZCharacterMoverComponent.h"

DEFINE_LOG_CATEGORY(LogZCharacter)

const FName AZCharacter::NAME_MontageSectionStart(FName("Start"));
const FName AZCharacter::NAME_MontageSectionEnd(FName("End"));

const FName AZCharacter::NAME_Pelvis(FName("pelvis"));
const FName AZCharacter::NAME_Hand_r_prop(FName("hand_r_prop"));
const FName AZCharacter::NAME_Hand_l_prop(FName("hand_l_prop"));

const FGuid AZCharacter::FZSaveVersion::GUID(0xA1B2C3D4, 0xE5F60708, 0x11121314, 0x15161718);

FCustomVersionRegistration GRegisterZCharacterSaveVersion(
    AZCharacter::FZSaveVersion::GUID,
    AZCharacter::FZSaveVersion::LatestVersion,
    TEXT("ZCharacterSaveVersion"));

static const TMap<EZCharacterActivity, EZCharacterActivity> CompatibleActivities =
{
	{ EZCharacterActivity::Sitting, EZCharacterActivity::Smoking | EZCharacterActivity::Talking },
	{ EZCharacterActivity::Smoking, EZCharacterActivity::Sitting | EZCharacterActivity::Talking },
	{ EZCharacterActivity::Talking, EZCharacterActivity::Sitting | EZCharacterActivity::Smoking },
};

AZCharacter::AZCharacter()
{
	static FName CapsuleComponentName("CapsuleComponent");
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(CapsuleComponentName);
	CapsuleComponent->InitCapsuleSize(34.0f, 88.0f);
	CapsuleComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	
	CapsuleComponent->CanCharacterStepUpOn = ECB_No;
	CapsuleComponent->SetShouldUpdatePhysicsVolume(true);
	CapsuleComponent->SetCanEverAffectNavigation(false);
	CapsuleComponent->bDynamicObstacle = true;
	RootComponent = CapsuleComponent;
	
	static FName MeshComponentName("MeshComponent");
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(MeshComponentName);
	MeshComponent->AlwaysLoadOnClient = true;
	MeshComponent->AlwaysLoadOnServer = true;
	MeshComponent->bOwnerNoSee = false;
	MeshComponent->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	MeshComponent->bCastDynamicShadow = true;
	MeshComponent->bAffectDynamicIndirectLighting = true;
	MeshComponent->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	MeshComponent->SetupAttachment(CapsuleComponent);
	static FName MeshCollisionProfileName("CharacterMesh");
	MeshComponent->SetCollisionProfileName(MeshCollisionProfileName);
	MeshComponent->SetGenerateOverlapEvents(false);
	MeshComponent->SetCanEverAffectNavigation(false);
	
	static FName InventoryComponentName(TEXT("InventoryComponent"));
	InventoryComponent = CreateDefaultSubobject<UZInventoryComponent>(InventoryComponentName);

	static FName EquipmentComponentName(TEXT("EquipmentComponent"));
	EquipmentComponent = CreateDefaultSubobject<UZEquipmentComponent>(EquipmentComponentName);

	static FName StaminaComponentName(TEXT("StaminaComponent"));
	StaminaComponent = CreateDefaultSubobject<UZStaminaComponent>(StaminaComponentName);

	static FName SaveComponentName(TEXT("SaveComponent"));
	SaveComponent = CreateDefaultSubobject<UZSaveComponent>(SaveComponentName);

	static FName RightHandPropComponentName(TEXT("RightHandProp"));
	RightHandPropMesh = CreateDefaultSubobject<UStaticMeshComponent>(RightHandPropComponentName);
	RightHandPropMesh->SetupAttachment(MeshComponent);
	RightHandPropMesh->SetVisibility(false);
	RightHandPropMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static FName LeftHandPropComponentName(TEXT("LeftHandProp"));
	LeftHandPropMesh = CreateDefaultSubobject<UStaticMeshComponent>(LeftHandPropComponentName);
	LeftHandPropMesh->SetupAttachment(MeshComponent);
	LeftHandPropMesh->SetVisibility(false);
	LeftHandPropMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	MaxHealth = 100.f;

	MeleeDamage = 10.f;

	DamageHeadMultiplier = 2.f;
	DamageLimbMultiplier = .5f;

	KnockbackForce = 1500.f;
	KnockbackUpwardsBoost = 400.f;
	bCanRagdoll = true;
	RagdollDuration = 5.f;
	bRagdolling = false;
	RagdollExitTransform = FTransform::Identity;

	bReplicates = true;
	SetReplicatingMovement(false);	// Disable Actor-level movement replication, since our Mover component will handle it

	HeadBoneNames = {"neck_01", "head"};
	LimbBoneNames = 
	{
		"upperarm_twist_01_r",
		"clavicle_l",
		"upperarm_l",
		"lowerarm_l",
		"hand_l",
		"index_01_l",
		"index_02_l",
		"index_03_l",
		"middle_01_l",
		"middle_02_l",
		"middle_03_l",
		"pinky_01_l",
		"pinky_02_l",
		"pinky_03_l",
		"ring_01_l",
		"ring_02_l",
		"ring_03_l",
		"thumb_01_l",
		"thumb_02_l",
		"thumb_03_l",
		"lowerarm_twist_01_l",
		"upperarm_twist_01_l",
		"clavicle_r",
		"upperarm_r",
		"lowerarm_r",
		"hand_r",
		"index_01_r",
		"index_02_r",
		"index_03_r",
		"middle_01_r",
		"middle_02_r",
		"middle_03_r",
		"pinky_01_r",
		"pinky_02_r",
		"pinky_03_r",
		"ring_01_r",
		"ring_02_r",
		"ring_03_r",
		"thumb_01_r",
		"thumb_02_r",
		"thumb_03_r",
		"lowerarm_twist_01_r",
		"thigh_l",
		"calf_l",
		"calf_twist_01_l",
		"foot_l",
		"ball_l",
		"thigh_twist_01_l",
		"thigh_r",
		"calf_r",
		"calf_twist_01_r",
		"foot_r",
		"ball_r",
		"thigh_twist_01_r",
	};

	EyesSocketName = FName("eyes");

	DesiredGaitState = EZGaitState::Run;

	Level = 1;
	
	bMovementEnabled = true;
	bOrientRotationToMovement = true;
	bIsJumpJustPressed = false;
	bUseBaseRelativeMovement = true;
	
	bIsJumpJustPressed = false;
	bIsJumpPressed = false;
	bIsSlidePressed = false;
}

void AZCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SetHealth(MaxHealth, nullptr);
	}

	MeshOffset = MeshComponent->GetRelativeTransform();
	PelvicBoneOffset = MeshComponent->GetSocketLocation(NAME_Pelvis);

	if (IsLocallyControlled())
	{
		StaminaComponent->OnCanSprintChanged.BindUObject(this, &ThisClass::OnCanSprintChanged);
		UpdateGaitState();
	}
}

void AZCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	if (MeshComponent->GetSkeletalMeshAsset())
	{
		if (RightHandPropMesh->GetAttachSocketName() != NAME_Hand_r_prop)
		{
			RightHandPropMesh->AttachToComponent(MeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, NAME_Hand_r_prop);
		}
		if (LeftHandPropMesh->GetAttachSocketName() != NAME_Hand_l_prop)
		{
			LeftHandPropMesh->AttachToComponent(MeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, NAME_Hand_l_prop);
		}
	}
}

void AZCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (USceneComponent* UpdatedComponent = MoverComponentBP ? MoverComponentBP->GetUpdatedComponent() : nullptr)
	{
		UpdatedComponent->SetCanEverAffectNavigation(bCanAffectNavigationGeneration);
	}
}

void AZCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

}

FVector AZCharacter::GetVelocity() const
{
	return MoverComponentBP ? MoverComponentBP->GetVelocity() : FVector::ZeroVector;
}

void AZCharacter::BeginMeleeDamage(EZZombieAttackType Type)
{
	// #ZTODO
}

void AZCharacter::EndMeleeDamage(EZZombieAttackType Type)
{
	// #ZTODO
}

void AZCharacter::MeleeHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this)
	{
		return;
	}
	if (DamagedActors.Contains(OtherActor))
	{
		return;
	}
	DamagedActors.Add(OtherActor);

	const FVector Direction = (OtherActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	UGameplayStatics::ApplyPointDamage(OtherActor, MeleeDamage, Direction, SweepResult, GetController(), this, nullptr);
}

void AZCharacter::Die(AController* DeathInstigator)
{
	for (const TPair<EZCharacterActivity, TScriptInterface<IZInteractable>>& KVP : InteractionTargets)
	{
		StopInteractingWith(KVP.Value, true);
	}
	bDead = true;

	SetMovementEnabled(false);
	MoverComponentBP->SetGravityOverride(true, FVector::ZeroVector);
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetCollisionResponseToChannel(ECC_Interactable, ECollisionResponse::ECR_Block); // #ZTODO belongs in AZZombieCharacter?

	if (ensure(HasAuthority()))
	{
		AZGameMode* GM = GetWorld()->GetAuthGameMode<AZGameMode>();
		GM->OnCharacterKilled(this, DeathInstigator);
	}

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Ragdoll);
	OnDeath.Broadcast();

	if (UAnimMontage* DeathMontage = AnimationData->DeathMontage)
	{
		if (UAnimInstance* AnimInst = MeshComponent->GetAnimInstance())
		{
			const float Duration = AnimInst->Montage_Play(DeathMontage);
		}
	}
}

void AZCharacter::SetHealth(float InHealth, AController* DamageInstigator)
{
	ensure(HasAuthority());

	Health = FMath::Clamp(InHealth, 0.f, MaxHealth);
	OnHealthChanged.Broadcast(Health, MaxHealth);

	if ((Health <= 0.f) && !bDead)
	{
		Die(DamageInstigator);
	}

	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, Health, this);
}

void AZCharacter::SetMaxHealth(float InMaxHealth)
{
	ensure(HasAuthority());

	MaxHealth = InMaxHealth;
	OnHealthChanged.Broadcast(Health, MaxHealth);

	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, MaxHealth, this);
}

void AZCharacter::ApplyKnockback(const FVector& ExplosionOrigin, float DamageScale)
{
	if (!MoverComponentBP)
	{
		return;
	}
	FVector Direction = GetActorLocation() - ExplosionOrigin;
	Direction.Normalize();

	const FVector LaunchVelocity = Direction * DamageScale * KnockbackForce + FVector::UpVector * KnockbackUpwardsBoost;

	if (bCanRagdoll)
	{
		EnterRagdoll();
		Multicast_Reliable_EnterRagdoll(LaunchVelocity);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Ragdoll, this, &ThisClass::ExitRagdoll, RagdollDuration);

		MeshComponent->AddImpulseToAllBodiesBelow(LaunchVelocity, NAME_Pelvis, true, true);
	}
	else
	{
		//MoverComponentBP->Launch(LaunchVelocity, EChaosMoverVelocityEffectMode::Impulse); #ZTODOMOVER
	}
}

void AZCharacter::EnterRagdoll()
{
	SetMovementEnabled(false);

	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	MeshComponent->SetCollisionProfileName(TEXT("Ragdoll")); // #ZTODO reset this when exiting ragdoll
	MeshComponent->SetAllBodiesSimulatePhysics(true);
	MeshComponent->SetSimulatePhysics(true);
	MeshComponent->WakeAllRigidBodies();
	MeshComponent->bBlendPhysics = true;
	MeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

	bRagdolling = true;
}

void AZCharacter::ExitRagdoll()
{
	bRagdolling = false;

	UZAnimInstance* AnimInst = Cast<UZAnimInstance>(MeshComponent->GetAnimInstance());
	if (ensure(AnimInst))
	{
		AnimInst->SaveRagdollPoseSnapShot();
	}

	// Get pelvis world location
	FVector PelvisLocation;
	FRotator PelvisRotation;
	MeshComponent->GetSocketWorldLocationAndRotation(NAME_Pelvis, PelvisLocation, PelvisRotation);
	const FTransform MeshWorldTransform = MeshComponent->GetComponentTransform();
	const float CapsuleHalfHeight = CapsuleComponent->GetScaledCapsuleHalfHeight();
	const bool bFacingUp = PelvisRotation.Pitch > 0;
	
	FVector NewLocation;
	FRotator NewRotation(0.f, PelvisRotation.Yaw, 0.f);

	if (HasAuthority())
	{
		const FVector TraceStart = PelvisLocation + FVector::UpVector * CapsuleHalfHeight;
		const FVector TraceEnd = PelvisLocation - FVector::UpVector * 200.f;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		FHitResult OutHit;
		//FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(Capsule->GetScaledCapsuleRadius(), CapsuleHalfHeight);
		//if (GetWorld()->SweepSingleByChannel(OutHit2, TraceStart, TraceEnd, FQuat::Identity, ECC_WorldStatic, CapsuleShape, QueryParams))
		if (GetWorld()->LineTraceSingleByChannel(OutHit, TraceStart, TraceEnd, ECC_WorldStatic, QueryParams))
		{
			NewLocation = OutHit.Location + FVector::UpVector * CapsuleHalfHeight;
		}
		else
		{
			NewLocation = PelvisLocation + FVector::UpVector * CapsuleHalfHeight;
		}
		// Move capsule
		SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
		SetActorRotation(NewRotation, ETeleportType::TeleportPhysics);
	}
	else
	{
		ensure(!RagdollExitTransform.Equals(FTransform::Identity));
		SetActorLocation(RagdollExitTransform.GetLocation(), false, nullptr, ETeleportType::TeleportPhysics);
		SetActorRotation(RagdollExitTransform.GetRotation(), ETeleportType::TeleportPhysics);
		RagdollExitTransform = FTransform::Identity;
	}
	
	MeshComponent->SetSimulatePhysics(false);
	MeshComponent->SetAllBodiesSimulatePhysics(false);
	MeshComponent->bBlendPhysics = false;

	// Reattach mesh to capsule
	MeshComponent->AttachToComponent(CapsuleComponent, FAttachmentTransformRules::KeepWorldTransform);

	
	AnimInst->OnExitRagdoll(MeshWorldTransform * (MeshOffset * CapsuleComponent->GetComponentTransform()).Inverse());
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
	{
		ResetMesh();
	});

	// Restore collision
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// Restore movement
	SetMovementEnabled(true);

	if (HasAuthority())
	{
		Multicast_Reliable_ExitRagdoll(FTransform(NewRotation, NewLocation, FVector::OneVector));
	}
	GetUpFromRagdoll(bFacingUp);

	UE_LOG(LogZCharacter, Warning, TEXT("Exit ragdoll"));
}

void AZCharacter::Multicast_Reliable_EnterRagdoll_Implementation(const FVector& LaunchVelocity)
{
	if (HasAuthority())
	{
		return;
	}
	EnterRagdoll();

	MeshComponent->AddImpulseToAllBodiesBelow(LaunchVelocity, NAME_Pelvis, true, true);
}

void AZCharacter::Multicast_Reliable_ExitRagdoll_Implementation(const FTransform& NewTransform)
{
	if (HasAuthority())
	{
		return;
	}
	RagdollExitTransform = NewTransform;
	ExitRagdoll();
}

void AZCharacter::GetUpFromRagdoll(bool bFacingUp)
{
	if (HasAuthority())
	{
		SetMovementEnabled(false);
	}
	if (UAnimMontage* GetUpMontage = bFacingUp ? GetUpFromRagdollUp : GetUpFromRagdollDown)
	{
		if (UZAnimInstance* AnimInst = Cast<UZAnimInstance>(MeshComponent->GetAnimInstance()))
		{
			const float Duration = AnimInst->Montage_Play(GetUpMontage);
			if (ensure(Duration > 0.f))
			{
				FOnMontageEnded EndDelegate;
				EndDelegate.BindUObject(this, &ThisClass::FinishGettingUpFromRagdoll);
				AnimInst->Montage_SetEndDelegate(EndDelegate, GetUpMontage);
			}
		}
	}
}

void AZCharacter::FinishGettingUpFromRagdoll(UAnimMontage* Montage, bool bInterrupted)
{
	if (HasAuthority())
	{
		SetMovementEnabled(true);
	}
}

bool AZCharacter::IsLocalPlayerControlled() const
{
	return IsLocallyControlled() && IsPlayerControlled();
}

FGenericTeamId AZCharacter::GetGenericTeamId() const
{
	if (AZAIController* AIC = GetController<AZAIController>())
	{
		return AIC->GetGenericTeamId();
	}
	else
	{
		return FGenericTeamId::NoTeam;
	}
}

void AZCharacter::OnRep_MaxHealth()
{
	OnHealthChanged.Broadcast(Health, MaxHealth);
}

void AZCharacter::OnRep_Health()
{
	OnHealthChanged.Broadcast(Health, MaxHealth);

	if (Health <= 0.f)
	{
		Die(nullptr);
	}
}

void AZCharacter::OnRep_InteractionTargetActors()
{
	InteractionTargets.Empty();
	for (const FZActivityInteractionTarget& InteractionTarget : InteractionTargetActors)
	{
		if (ensure((InteractionTarget.Activity != EZCharacterActivity::None) && (InteractionTarget.InteractionTargetActor)))
		{
			InteractionTargets.Add(InteractionTarget.Activity, TScriptInterface<IZInteractable>(InteractionTarget.InteractionTargetActor));
		}
	}
}

void AZCharacter::OnRep_CurrentActivities(EZCharacterActivity OldActivities)
{
	const EZCharacterActivity StartedActivities = CurrentActivities & ~OldActivities;
	const EZCharacterActivity StoppedActivities = OldActivities & ~CurrentActivities;

	// Iterate through each bit
	for (uint32 Bit = 1; Bit < static_cast<uint32>(EZCharacterActivity::MAX); Bit <<= 1)
	{
		const EZCharacterActivity Activity = static_cast<EZCharacterActivity>(Bit);
		if (EnumHasAnyFlags(StartedActivities, Activity))
		{
			StartActivity(Activity);
		}
		if (EnumHasAnyFlags(StoppedActivities, Activity))
		{
			StopActivityLocal(Activity);
		}
	}
}

void AZCharacter::OnRep_EndingActivities(EZCharacterActivity OldEndingActivities)
{
	const EZCharacterActivity StartedEnding = EndingActivities & ~OldEndingActivities;

	for (uint32 Bit = 1; Bit < static_cast<uint32>(EZCharacterActivity::MAX); Bit <<= 1)
	{
		const EZCharacterActivity Activity = static_cast<EZCharacterActivity>(Bit);
		if (EnumHasAnyFlags(StartedEnding, Activity))
		{
			StopActivityMontage(Activity);
		}
	}
}

bool AZCharacter::ShouldTakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const
{
	if (bDead)
	{
		return false;
	}
	AZGameMode* GM = GetWorld()->GetAuthGameMode<AZGameMode>();
	if (ensure(GM))
	{
		const bool bIsThisPlayer = GetController() && GetController()->IsPlayerController();
		const bool bIsOtherPlayer = EventInstigator->IsPlayerController();
		if (!GM->GetAllowPlayerFriendlyFire())
		{
			if (bIsThisPlayer && bIsOtherPlayer)
			{
				return false;
			}
		}
		if (!GM->GetAllowNPCFriendlyFire())
		{
			if (!bIsThisPlayer && !bIsOtherPlayer)
			{
				if (AZCharacter* InstigatorCharacter = EventInstigator->GetPawn<AZCharacter>())
				{
					if (GetFaction() == InstigatorCharacter->GetFaction())
					{
						return false;
					}
				}
			}
		}
	}
	return true;
}

float AZCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if (ActualDamage <= 0.f)
	{
		return 0.f;
	}
	//if (DamageEvent.DamageTypeClass->IsChildOf(UZMeleeDamage::StaticClass())) #ZTODO
	//{
	//	// Apply level scaling from DamageCurve
	//}
	//else
	//{
	//	// Use raw damage amount from weapon data
	//}


	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent& PointDamageEvent = static_cast<const FPointDamageEvent&>(DamageEvent);
		const FName Bone = PointDamageEvent.HitInfo.BoneName;
		if(Bone != NAME_None)
		{
			if (HeadBoneNames.Contains(Bone))
			{
				ActualDamage *= DamageHeadMultiplier;
			}
			else if (LimbBoneNames.Contains(Bone))
			{
				ActualDamage *= DamageLimbMultiplier;
			}
		}
	}
	else if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		UE_LOG(LogZCharacter, Warning, TEXT("radial damage taken by: %s"), *GetName());
		const FRadialDamageEvent& RadialEvent = static_cast<const FRadialDamageEvent&>(DamageEvent);
		FVector Direction = GetActorLocation() - RadialEvent.Origin;
		const float Distance = Direction.Size();
		ApplyKnockback(RadialEvent.Origin, RadialEvent.Params.GetDamageScale(Distance));
	}

	SetHealth(FMath::Max(0.f, Health - ActualDamage), EventInstigator);

	return ActualDamage;
}

void AZCharacter::StartPickUp(AZInventoryItem* Item)
{
	if (HasAuthority())
	{
		Item->StartPickUp(this);

		//GetWorldTimerManager().SetTimer(TimerHandle_GameplayEvent, this, &ThisClass::PickUp, PickUpDuration, false);
		PickUp(Item);
	}
}

void AZCharacter::PickUp(AZInventoryItem* Item)
{
	if (HasAuthority())
	{
		if (Item)
		{
			GetInventoryComponent()->AddItem(Item->GetItemId(), Item->GetCount(), true);
			const EZCharacterActivity Activity = Item->GetCharacterActivity();
			ClearInteractionTarget(Activity);
			Item->Destroy();
		}
	}
}

void AZCharacter::SetADSing(bool bVal)
{
	bADSing = bVal;
	if (IsLocallyControlled())
	{
		UpdateGaitState();
	}
}

void AZCharacter::OnCrouchUpdate(float RelativeLocZ)
{
	// BaseTranslationOffset.Z = RelativeLocZ; #ZTODO
}

void AZCharacter::ResetMesh()
{
	MeshComponent->SetRelativeTransform(MeshOffset);
}

void AZCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, Health, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MaxHealth, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, CurrentActivities, Params); // Replicate on NPCs so that the players know if they can interact with them or not
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, EndingActivities, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, InteractionTargetActors, Params); // Replicate on NPCs so that the players know if they can interact with them or not

	FDoRepLifetimeParams ParamsSkipOwner;
	ParamsSkipOwner.bIsPushBased = true;
	ParamsSkipOwner.Condition = COND_SimulatedOnly;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bADSing, ParamsSkipOwner);
}

bool AZCharacter::SetInteractionTarget(TScriptInterface<IZInteractable> InInteractionTarget)
{
	if (!ensure(InInteractionTarget))
	{
		return false;
	}
	const EZCharacterActivity Activity = InInteractionTarget->GetCharacterActivity();
	if (InteractionTargets.Contains(Activity))
	{
		return false;
	}
	InteractionTargets.Add(Activity, InInteractionTarget);
	InteractionTargetActors.Add(FZActivityInteractionTarget(Activity, Cast<AActor>(InInteractionTarget.GetObject())));
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, InteractionTargetActors, this);

	OnInteractionStarted.Broadcast(this, InInteractionTarget);
	return true;
}

void AZCharacter::ClearInteractionTarget(EZCharacterActivity Activity)
{
	if (TScriptInterface<IZInteractable> FoundTarget = GetInteractionTarget(Activity))
	{
		OnInteractionStopped.Broadcast(this, FoundTarget);
		InteractionTargets.Remove(Activity);
	}
}

bool AZCharacter::IsEndingActivity(EZCharacterActivity Activity) const
{
	return EnumHasAnyFlags(EndingActivities, Activity);
}

void AZCharacter::SetMovementEnabled(bool bEnabled)
{
	bMovementEnabled = bEnabled;
	MoverComponentBP->SetActive(bEnabled);
}

void AZCharacter::Jump()
{
	bIsJumpJustPressed = !bIsJumpPressed;
	bIsJumpPressed = true;
}

void AZCharacter::JumpEnd()
{
	bIsJumpPressed = false;
	bIsJumpJustPressed = false;
}

void AZCharacter::Slide()
{
	bIsSlidePressed = true;
}

void AZCharacter::SlideEnd()
{
	bIsSlidePressed = false;
}

EZMovementMode AZCharacter::GetCurrentMovementMode() const
{
	if (!MoverComponentBP)
	{
		return EZMovementMode::Falling;
	}
	return UZGameplayStatics::GetMovementMode(MoverComponentBP->GetMovementModeName());
}

TScriptInterface<IZInteractable> AZCharacter::GetInteractionTarget(EZCharacterActivity Activity) const
{
	if (const TScriptInterface<IZInteractable>* InteractionTarget = InteractionTargets.Find(Activity))
	{
		return *InteractionTarget;
	}
	return nullptr;
}

FTransform AZCharacter::GetPivotPoint() const
{
	return GetActorTransform();
}

EZCharacterActivity AZCharacter::GetCharacterActivity() const
{
	unimplemented(); return EZCharacterActivity::None;
}

FZInteractionParams AZCharacter::GetInteractionParams(const AZCharacter* InCharacter) const
{
	if (GetCharacterActivity() == EZCharacterActivity::Looting)
	{
		return FZInteractionParams(TInPlaceType<FZLootingParams>{}, InventoryComponent);
	}
	return FZInteractionParams(TInPlaceType<FZNoInteractionParams>{});
}

void AZCharacter::CrouchStart()
{
}

void AZCharacter::CrouchEnd()
{
}

void AZCharacter::DoUnequip()
{
	EquipmentComponent->StartUnequip();
}

void AZCharacter::SetMovementState(EZGaitState InGaitState)
{
	if (DesiredGaitState == InGaitState)
	{
		return;
	}
	DesiredGaitState = InGaitState;
	UpdateGaitState();
}

AActor* AZCharacter::GetLookAtTarget() const
{
	if (AZAIController* AIController = GetController<AZAIController>())
	{
		if (AZCharacter* CombatTarget = AIController->GetCombatTarget())
		{
			return CombatTarget;
		}
	}
	return LookAtTarget;
}

UAISense_Sight::EVisibilityResult AZCharacter::CanBeSeenFrom(const FCanBeSeenFromContext& Context, FVector& OutSeenLocation, int32& OutNumberOfLoSChecksPerformed, int32& OutNumberOfAsyncLosCheckRequested, float& OutSightStrength, int32* UserData, const FOnPendingVisibilityQueryProcessedDelegate* Delegate)
{
	if (bDead)
	{
		return UAISense_Sight::EVisibilityResult::NotVisible;
	}
    const AActor* ObserverActor = Context.IgnoreActor; // IgnoreActor is actually the observer's pawn
    if (const AZCharacter* ObserverCharacter = Cast<AZCharacter>(ObserverActor))
    {
        ETeamAttitude::Type Attitude = FGenericTeamId::GetAttitude(ObserverCharacter->GetGenericTeamId(), GetGenericTeamId());

        // Neutral and friendly NPCs are invisible to other neutral/friendly NPCs
        if (((Attitude == ETeamAttitude::Neutral) || (Attitude == ETeamAttitude::Friendly)) && !IsPlayerControlled())
        {
            return UAISense_Sight::EVisibilityResult::NotVisible;
        }
    }
	
	FVector TargetEyeLocation;
	FRotator TargetEyeRotation;
	GetActorEyesViewPoint(TargetEyeLocation, TargetEyeRotation);
	OutSeenLocation = TargetEyeLocation;

    OutNumberOfLoSChecksPerformed = 1;

    FHitResult HitResult;
    const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Context.ObserverLocation, TargetEyeLocation, ECC_Visibility, FCollisionQueryParams(NAME_None, false, ObserverActor));

    if (!bHit || (HitResult.GetActor() == this)) // #ZTODO use UE::AISense_Sight::IsTraceConsideredVisible?
    {
        OutSightStrength = 1.f;
        return UAISense_Sight::EVisibilityResult::Visible;
    }
	else
	{
		OutSightStrength = 0.f;
		return UAISense_Sight::EVisibilityResult::NotVisible;
	}
}

void AZCharacter::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	if (Ar.IsSaveGame())
    {
        const int32 Version = Ar.CustomVer(FZSaveVersion::GUID);

        //if (Ar.IsLoading())
        //{
        //    if (Version < FZSaveVersion::AddedStamina)
        //    {
        //        Stamina = 100.f; // default for old saves
        //    }
        //}

        Ar << Health;
		Ar << MaxHealth;
    }
}

void AZCharacter::StartInteractionActivity(const FZInteractionState& InInteractionState)
{
	ensure(HasAuthority());
	
	StartActivity(InInteractionState.Activity);

	OnInteractionActivityStarted.Broadcast(InInteractionState);
}

void AZCharacter::StartActivity(EZCharacterActivity Activity)
{
	if (HasAuthority())
	{
		if (Activity == EZCharacterActivity::None)
		{
			ensure(0);
			return;
		}
		if (CurrentActivities != EZCharacterActivity::None)
		{
			const bool bCompatible = IsActivityCompatible(Activity);
			//ensureMsgf(bCompatible, TEXT("Incompatible activities: trying to start %s while %s is active"),
			//	*ActivityToString(InInteractionState.Activity), *ActivityToString(IncompatibleActivities));
			if (!ensure(bCompatible))
			{
				return;
			}
		}

		if ((Activity & CurrentActivities) != EZCharacterActivity::None)
		{
			ensure(0); // Already performing that activity
			return;
		}

		CurrentActivities |= Activity;
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, CurrentActivities, this);
	}

	switch (Activity)
	{
		case EZCharacterActivity::Sleeping:
			SetSleeping(true);
			break;
		case EZCharacterActivity::Sitting:
			SetSitting(true);
			break;
		case EZCharacterActivity::Looting:
			SetLooting(true);
			break;
		case EZCharacterActivity::Lockpicking:
			break;
		case EZCharacterActivity::Talking:
			break;
		case EZCharacterActivity::Smoking:
			break;
		case EZCharacterActivity::PickingUp:
			break;
		case EZCharacterActivity::Surrendering:
			break;
		case EZCharacterActivity::None:
		default:
			ensure(0);
			break;
	}
	
	PlayActivityMontage(Activity);

	if (HasAuthority())
	{
		UpdateGaitState();
	}
}

void AZCharacter::UpdateActivity(const FZInteractionState& InInteractionState)
{
	OnInteractionActivityUpdated.Broadcast(InInteractionState);
}

bool AZCharacter::TriggerEndingActivity(EZCharacterActivity Activity, bool bForceInstant)
{
	const bool bIsEndingActivity = IsEndingActivity(Activity);
	if (!IsPerformingActivity(Activity) || bIsEndingActivity) // Prevent spam
	{
		if (bIsEndingActivity && bForceInstant)
		{
			ForceStopActivityMontage(Activity);
			return true;
		}
		return !bIsEndingActivity;
	}
	if (EndsActivityInstantly(Activity))
	{
		StopActivity(Activity);
		return true;
	}
	else
	{
		if (bForceInstant)
		{
			ForceStopActivityMontage(Activity);
			return true;
		}
		else
		{
			StopActivityMontage(Activity);
			return false;
		}
	}
}

void AZCharacter::StopActivity(EZCharacterActivity Activity)
{
	ensure(HasAuthority());
	if (Activity == EZCharacterActivity::None)
	{
		ensure(0);
		return;
	}
	if ((CurrentActivities & Activity) == EZCharacterActivity::None)
	{
		ensure(0); // Trying to stop an activity that's not being performed
		return;
	}
	CurrentActivities &= ~Activity;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, CurrentActivities, this);
	if (EnumHasAnyFlags(EndingActivities, Activity))
	{
		EndingActivities &= ~Activity;
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, EndingActivities, this);
	}

	StopActivityLocal(Activity);

	if (TScriptInterface<IZInteractable> InteractionTarget = GetInteractionTarget(Activity)) // Interaction activity
	{
		InteractionTarget->EndInteract(this);
		ClearInteractionTarget(Activity);
	}

	UpdateGaitState();

	OnActivityStopped.Broadcast(Activity);
}

void AZCharacter::ActivityEnding(EZCharacterActivity Activity)
{
	if (IsEndingActivity(Activity))
	{
		return;
	}
	if (HasAuthority())
	{
		EndingActivities |= Activity;
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, EndingActivities, this);
	}
	OnActivityEnding.Broadcast(Activity);
}

void AZCharacter::StopActivityLocal(EZCharacterActivity Activity)
{
	switch (Activity)
	{
		case EZCharacterActivity::Sleeping:
			SetSleeping(false);
			break;
		case EZCharacterActivity::Sitting:
			SetSitting(false);
			break;
		case EZCharacterActivity::Looting:
			SetLooting(false);
			break;
		case EZCharacterActivity::Lockpicking:
			break;
		case EZCharacterActivity::Talking:
			break;
		case EZCharacterActivity::Smoking:
			break;
		case EZCharacterActivity::PickingUp:
			break;
		case EZCharacterActivity::Surrendering:
			break;
		case EZCharacterActivity::None:
		default:
			ensure(0);
			break;
	}
}

bool AZCharacter::EndsActivityInstantly(EZCharacterActivity Activity) const
{
	return !AnimationData->ActivityMontages.Contains(Activity);
}

void AZCharacter::PlayActivityMontage(EZCharacterActivity Activity)
{
	UAnimMontage* ActivityMontage = AnimationData->ActivityMontages.FindRef(Activity);
	if (!ActivityMontage)
	{
		return;
	}
	if (UAnimInstance* AnimInstance = MeshComponent->GetAnimInstance())
	{
		const float Duration = AnimInstance->Montage_Play(ActivityMontage, 1.f, EMontagePlayReturnType::Duration, 0.f, false);
		if (HasAuthority())
		{
			if (ensure(Duration > 0.f))
			{
				FOnMontageEnded EndDelegate;
				EndDelegate.BindLambda([WeakThis = MakeWeakObjectPtr(this), Activity](UAnimMontage*, bool)
				{
					if (AZCharacter* StrongThis = WeakThis.Get())
					{
						StrongThis->OnEndMontageEnded(Activity);
					}
				});
				AnimInstance->Montage_SetEndDelegate(EndDelegate, ActivityMontage);
				FOnMontageSectionChanged SectionChangedDelegate;
				SectionChangedDelegate.BindLambda([WeakThis = MakeWeakObjectPtr(this), Activity](UAnimMontage* Montage, FName SectionName, bool bInterrupted)
				{
					if (AZCharacter* StrongThis = WeakThis.Get())
					{
						if (SectionName == NAME_MontageSectionEnd)
						{
							StrongThis->ActivityEnding(Activity);
						}
					}
				});
				AnimInstance->Montage_SetSectionChangedDelegate(SectionChangedDelegate, ActivityMontage);
			}
		}
	}
}

void AZCharacter::StopActivityMontage(EZCharacterActivity Activity)
{
	ActivityEnding(Activity);

	UAnimMontage* ActivityMongate = AnimationData->ActivityMontages.FindRef(Activity);
	if (UAnimInstance* AnimInstance = MeshComponent->GetAnimInstance())
	{
		FAnimMontageInstance* ActivityAnimMontageInstance = AnimInstance->GetActiveInstanceForMontage(ActivityMongate);
		if (!ensure(ActivityAnimMontageInstance))
		{
			if (HasAuthority())
			{
				StopActivity(Activity);
			}
			return;
		}
		const FName CurrentSection = ActivityAnimMontageInstance->GetCurrentSection();
		if (CurrentSection == NAME_MontageSectionStart) // If we are still in the starting section of the montage finish it first and then jump to the end section
		{
			ActivityAnimMontageInstance->OnMontageSectionChanged.BindLambda([WeakThis = MakeWeakObjectPtr(this), Activity](UAnimMontage* Montage, FName SectionName, bool bInterrupted)
			{
				if (AZCharacter* StrongThis = WeakThis.Get())
				{
					UAnimInstance* AnimInstance = StrongThis->MeshComponent->GetAnimInstance();
					UAnimMontage* ActivityMongate = StrongThis->AnimationData->ActivityMontages.FindRef(Activity);
					FAnimMontageInstance* ActivityAnimMontageInstance = AnimInstance->GetActiveInstanceForMontage(ActivityMongate);
					if (ActivityAnimMontageInstance)
					{
						if (ActivityAnimMontageInstance->GetCurrentSection() != NAME_MontageSectionEnd)
						{
							ActivityAnimMontageInstance->JumpToSectionName(NAME_MontageSectionEnd);
						}
						ActivityAnimMontageInstance->OnMontageSectionChanged.Unbind();
					}
				}
			});
		}
		else
		{
			ActivityAnimMontageInstance->JumpToSectionName(NAME_MontageSectionEnd);
		}
	}
}

void AZCharacter::OnEndMontageEnded(EZCharacterActivity Activity)
{
	if (HasAuthority())
	{
		StopActivity(Activity);
	}
}

void AZCharacter::ForceStopActivityMontage(EZCharacterActivity Activity)
{
	UAnimMontage* ActivityMongate = AnimationData->ActivityMontages.FindRef(Activity);
	if (ensure(ActivityMongate))
	{
		if (UAnimInstance* AnimInstance = MeshComponent->GetAnimInstance())
		{
			FAnimMontageInstance* ActivityAnimMontageInstance = AnimInstance->GetActiveInstanceForMontage(ActivityMongate);
			if (ActivityAnimMontageInstance)
			{
				ActivityAnimMontageInstance->OnMontageEnded.ExecuteIfBound(ActivityMongate, true);
				ActivityAnimMontageInstance->OnMontageEnded = FOnMontageEnded();
			}
			AnimInstance->Montage_Stop(0.f, ActivityMongate);
		}
	}
}

void AZCharacter::SetSleeping(bool bSleeping)
{
	if (HasAuthority())
	{
		CapsuleComponent->SetCollisionResponseToChannel(ECC_Pawn, bSleeping ? ECR_Ignore : ECR_Block);
		if (bSleeping)
		{
			SetMovementEnabled(false);
		}
		else
		{
			SetMovementEnabled(true);
		}
	}
	
	if (bSleeping)
	{
		const TScriptInterface<IZInteractable>* FoundBed = InteractionTargets.Find(EZCharacterActivity::Sleeping);
		const AZBed* Bed = FoundBed ? Cast<AZBed>(FoundBed->GetObject()) : nullptr;
		ensure(Bed);
		bMirrorAnimation = Bed && !Bed->GetRightSide();
	}
}

void AZCharacter::SetSitting(bool bSitting)
{
	if (HasAuthority())
	{
		CapsuleComponent->SetCollisionResponseToChannel(ECC_Pawn, bSitting ? ECR_Ignore : ECR_Block);
		if (bSitting)
		{
			SetMovementEnabled(false);
		}
		else
		{
			SetMovementEnabled(true);
		}
	}
}

void AZCharacter::SetLooting(bool bLooting)
{
	// #ZTODO NPC looting
}

void AZCharacter::UpdateGaitState()
{
	// if (!ZCharacterMovement)
	// {
	// 	return;
	// }
	// const EZGaitState EffectiveGaitState = GetEffectiveGaitState();
	// if (bADSing)
	// {
	// 	ZCharacterMovement->SetGaitState((EZGaitState)FMath::Min((uint8)EffectiveGaitState, (uint8)EZGaitState::Walk));
	// }
	// else
	// {
	// 	if (!StaminaComponent->CanSprint() && (EffectiveGaitState == EZGaitState::Sprint))
	// 	{
	// 		ZCharacterMovement->SetGaitState(EZGaitState::Run);
	// 	}
	// 	else
	// 	{
	// 		ZCharacterMovement->SetGaitState(EffectiveGaitState);
	// 	}
	// }
}

EZGaitState AZCharacter::GetEffectiveGaitState() const
{
	const EZGaitState MaxAllowedGaitState = GetMaxAllowedGaitState();
	return static_cast<EZGaitState>(FMath::Min(static_cast<uint8>(DesiredGaitState), static_cast<uint8>(MaxAllowedGaitState)));
}

EZGaitState AZCharacter::GetMaxAllowedGaitState() const
{
	if (EnumHasAnyFlags(CurrentActivities,
		EZCharacterActivity::Talking |
		EZCharacterActivity::Looting |
		EZCharacterActivity::Lockpicking |
		EZCharacterActivity::Sitting))
	{
		return EZGaitState::None;
	}
	if (EnumHasAnyFlags(CurrentActivities,
		EZCharacterActivity::Smoking |
		EZCharacterActivity::Surrendering))
	{
		return EZGaitState::Walk;
	}
	return EZGaitState::Sprint;
}

void AZCharacter::OnCanSprintChanged()
{
	UpdateGaitState();
}

void AZCharacter::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	OutLocation = MeshComponent->GetSocketLocation(EyesSocketName);
	OutRotation = MeshComponent->GetSocketRotation(EyesSocketName);
}

void AZCharacter::GetAimViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	GetActorEyesViewPoint(OutLocation, OutRotation);
}

bool AZCharacter::IsActivityCompatible(EZCharacterActivity Activity) const
{
	if (const EZCharacterActivity* Compatible = CompatibleActivities.Find(Activity))
	{
		return (CurrentActivities & ~*Compatible) == EZCharacterActivity::None;
	}
	else
	{
		return (CurrentActivities == EZCharacterActivity::None);
	}
}

bool AZCharacter::CanInteract(const AZCharacter* InCharacter) const
{
	const EZCharacterActivity Activity = GetCharacterActivity();
	ensure(IsInteractionActivity(Activity));
	return !InteractionTargets.Contains(Activity) && InCharacter->IsActivityCompatible(Activity) ;
}

bool AZCharacter::BeginInteract(AZCharacter* InCharacter) // InCharacter starts interacting with this
{
	if (!CanInteract(InCharacter))
	{
		return false;
	}
	if (!ensure(SetInteractionTarget(InCharacter)))
	{
		// CanInteract() should have returned false in this case
		return false;
	}
	return true;
}

void AZCharacter::EndInteract(AZCharacter* InCharacter)
{
}

bool AZCharacter::StartInteractingWith(TScriptInterface<IZInteractable> InInteractionTarget)
{
	if (!ensure(HasAuthority()))
	{
		return false;
	}
	if (!InInteractionTarget)
	{
		ensure(0);
		return false;
	}
	if (!InInteractionTarget->CanInteract(this)) // Can this character interact with InInteractionTarget?
	{
		if (AZNPCharacter* TargetNPC = Cast<AZNPCharacter>(InInteractionTarget.GetObject()))
		{
			//if (IsActivityCompatible(TargetNPC->GetCharacterActivity())) // Additional check if we can even interact with the TargetNPC
			{
				TargetNPC->OnInteractionAttempted(this);
			}
		}
		return false;
	}
	AZCharacter* InteractionTargetCharacter = Cast<AZCharacter>(InInteractionTarget.GetObject());
	if (InteractionTargetCharacter && InteractionTargetCharacter->ShouldInteractBack())
	{
		if (!CanInteract(InteractionTargetCharacter)) // Can InteractionTargetCharacter interact with this?
		{
			return false;
		}
		const bool InteractionResult = BeginInteract(InteractionTargetCharacter); // Make InteractionTargetCharacter interact with this
		if (ensure(InteractionResult))
		{
			InteractionTargetCharacter->StartInteractionActivity(FZInteractionState(this, GetCharacterActivity(), GetInteractionParams(InteractionTargetCharacter)));
		}
		else
		{
			return false;
		}
	}
	else
	{
		if (!SetInteractionTarget(InInteractionTarget)) // We don't check if InInteractionTarget can interact with this, because it's an inanimate object, we just checks if we're already doing something with CharacterActivity that InInteractionTarget->GetCharacterActivity() returns
		{
			return false;
		}
	}
	// Now this starts interacting with InInteractionTarget
	const bool InteractionResult = InInteractionTarget->BeginInteract(this);
	if (ensure(InteractionResult))
	{
		StartInteractionActivity(FZInteractionState(InInteractionTarget, InInteractionTarget->GetCharacterActivity(), InInteractionTarget->GetInteractionParams(this)));
	}
	else
	{
		if (InteractionTargetCharacter && InteractionTargetCharacter->ShouldInteractBack())
		{
			InteractionTargetCharacter->TriggerEndingActivity(GetCharacterActivity(), true);
		}
	}
	return InteractionResult;
}

void AZCharacter::StopInteractingWith(TScriptInterface<IZInteractable> InInteractionTarget, bool bForceInstant)
{
	if (!ensure(HasAuthority()))
	{
		return;
	}
	if (!ensure(InInteractionTarget))
	{
		return;
	}
	AZCharacter* InteractionTargetCharacter = Cast<AZCharacter>(InInteractionTarget.GetObject());
	if (InteractionTargetCharacter && InteractionTargetCharacter->ShouldInteractBack())
	{
		const EZCharacterActivity ThisActivity = GetCharacterActivity();
		InteractionTargetCharacter->TriggerEndingActivity(ThisActivity);
	}
	
	const EZCharacterActivity InteractableActivity = InInteractionTarget->GetCharacterActivity();
	TriggerEndingActivity(InteractableActivity, bForceInstant);
}

TArray<UMeshComponent*> AZCharacter::GetHighlightMeshes() const
{
	return { MeshComponent };
}


