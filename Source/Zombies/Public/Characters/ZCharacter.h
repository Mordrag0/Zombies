// Copyright 2026 Luka MarkuÃ…Â¡ All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameplayTagContainer.h"
#include "MoverSimulationTypes.h"
#include "ZInteractable.h"
#include "Perception/AISightTargetInterface.h"
#include "ZCharacter.generated.h"

class UCharacterMoverComponent;
class UCapsuleComponent;
class USkeletalMeshComponent;
class UZInventoryComponent;
class AZInventoryItem;
class AZEquippableItem;
class UZStaminaComponent;
class UZCharacterMoverComponent;
class UAnimMontage;
class UZEquipmentComponent;
class UZSaveComponent;
class IZInteractable;
class AZCharacter;
struct FDamageEvent;
class UStaticMeshComponent;
class UZCharacterAnimationData;
enum class EZCharacterActivity : uint32;
enum class EZFaction : uint8;
enum class EZZombieAttackType : uint8;
enum class EZSkill : uint8;
enum class EZMovementMode : uint8;

DECLARE_LOG_CATEGORY_EXTERN(LogZCharacter, Log, All);

DECLARE_MULTICAST_DELEGATE(FZOnDeath);
DECLARE_MULTICAST_DELEGATE_TwoParams(FZOnHealthChanged, float, float);

DECLARE_MULTICAST_DELEGATE_OneParam(FZonActivityStarted, const FZInteractionState& InteractionState);
DECLARE_MULTICAST_DELEGATE_OneParam(FZOnActivityUpdated, const FZInteractionState& InteractionState);
DECLARE_MULTICAST_DELEGATE_OneParam(FZonActivityStopped, EZCharacterActivity);
DECLARE_DELEGATE_OneParam(FZOnCharacterEvent, FGameplayTag)

UENUM(BlueprintType)
enum class EZGaitState : uint8 // Must be ordered from slowest to fastest
{
	None,
	Walk,
	Run,
	Sprint
};

USTRUCT()
struct FZActivityInteractionTarget
{
	GENERATED_BODY()

	UPROPERTY()
	EZCharacterActivity Activity = static_cast<EZCharacterActivity>(0);

	UPROPERTY()
	TObjectPtr<AActor> InteractionTargetActor = nullptr;
};

UCLASS()
class ZOMBIES_API AZCharacter 
	: public APawn
	, public IZInteractable
	, public IAISightTargetInterface
	, public IMoverInputProducerInterface
{
	GENERATED_BODY()

public:
    struct FZSaveVersion
    {
        static const FGuid GUID;

        enum Type
        {
            Initial = 0,
            //AddedStamina,
            //AddedMana,
            VersionPlusOne,
            LatestVersion = VersionPlusOne - 1
        };
    };

	AZCharacter();
	
	virtual void BeginPlay() override;
	
	virtual void OnConstruction(const FTransform& Transform) override;
	
	virtual void PostInitializeComponents() override;
	
	virtual void PossessedBy(AController* NewController) override;
	
	virtual FVector GetVelocity() const override;

	virtual void BeginMeleeDamage(EZZombieAttackType Type);

	virtual void EndMeleeDamage(EZZombieAttackType Type);

	virtual bool ShouldTakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const override;

	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UZInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

	UZEquipmentComponent* GetEquipmentComponent() const { return EquipmentComponent; }

	UZStaminaComponent* GetStaminaComponent() const { return StaminaComponent; }

	virtual USkeletalMeshComponent* GetActualMesh() const { return MeshComponent; }

	virtual void StartPickUp(AZInventoryItem* Item);

	void PickUp(AZInventoryItem* Item);

	bool GetDead() const { return bDead; }

	bool GetHasInfiniteAmmo() const { return bHasInfiniteAmmo; }

	virtual void SetADSing(bool bVal);
	bool GetADSing() const { return bADSing; }

	float GetHealth() const { return Health; }
	float GetMaxHealth() const { return MaxHealth; }

	virtual void OnCrouchUpdate(float RelativeLocZ);

	void ResetMesh();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual bool CanInteract(const AZCharacter* InCharacter) const override;
	virtual bool BeginInteract(AZCharacter* InCharacter) override;
	virtual void EndInteract(AZCharacter* InCharacter) override;

	virtual TArray<UMeshComponent*> GetHighlightMeshes() const override;
	virtual FTransform GetPivotPoint() const override;
	virtual EZCharacterActivity GetCharacterActivity() const override;
	virtual FZInteractionParams GetInteractionParams(const AZCharacter* InCharacter) const override;

	bool ShouldInteractBack() const { return !bDead; }
	virtual TScriptInterface<IZInteractable> GetInteractionTarget(EZCharacterActivity Activity) const;

	virtual bool StartInteractingWith(TScriptInterface<IZInteractable> InInteractionTarget);
	virtual void StopInteractingWith(TScriptInterface<IZInteractable> InInteractionTarget, bool bForceInstant = false);

	EZFaction GetFaction() const { return Faction; }

	void CrouchStart();
	void CrouchEnd();

	void DoUnequip();

	void SetMovementState(EZGaitState InMovementState);

	AActor* GetLookAtTarget() const;
	void ClearLookAtTarget() { LookAtTarget = nullptr; }
	void SetLookAtTarget(AActor* InTarget) { LookAtTarget = InTarget; }

	virtual UAISense_Sight::EVisibilityResult CanBeSeenFrom(const FCanBeSeenFromContext& Context, FVector& OutSeenLocation, int32& OutNumberOfLoSChecksPerformed, int32& OutNumberOfAsyncLosCheckRequested, float& OutSightStrength, int32* UserData = nullptr, const FOnPendingVisibilityQueryProcessedDelegate* Delegate = nullptr) override;

	virtual void Serialize(FArchive& Ar) override;

	void SetMirrorAnimation(bool bInMirrorAnimation) { bMirrorAnimation = bInMirrorAnimation; }
	bool GetMirrorAnimation() const { return bMirrorAnimation; }

	bool IsPerformingActivity(EZCharacterActivity Activity) const { return EnumHasAnyFlags(CurrentActivities, Activity); }

	EZCharacterActivity GetCurrentActivities() const { return CurrentActivities; }

	virtual bool HasSkill(EZSkill Skill) const { return false; }

	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;
	virtual void GetAimViewPoint(FVector& OutLocation, FRotator& OutRotation) const;

	bool IsActivityCompatible(EZCharacterActivity Activity) const;
	virtual void StartInteractionActivity(const FZInteractionState& InInteractionState);
	void StartActivity(EZCharacterActivity Activity);
	virtual void UpdateActivity(const FZInteractionState& InInteractionState);
	bool TriggerEndingActivity(EZCharacterActivity Activity, bool bForceInstant = false); // Returns whether the activity was stopped immediately
	bool IsEndingActivity(EZCharacterActivity Activity) const;

	int32 GetCharacterLevel() const { return Level; }

	virtual FGameplayTag GetDeathEvent() const { return FGameplayTag::EmptyTag; }

	void SetMovementEnabled(bool bEnabled);

	USkeletalMeshComponent* GetMesh() const { return MeshComponent; }
	UCapsuleComponent* GetCapsuleComponent() const { return CapsuleComponent; }
	UZCharacterMoverComponent* GetMoverComponent() const { return MoverComponentBP; }
	
	void Jump();
	void JumpEnd();
	
	void Slide();
	void SlideEnd();
	
	EZMovementMode GetCurrentMovementMode() const;
	
	FZonActivityStarted OnInteractionActivityStarted;
	FZOnActivityUpdated OnInteractionActivityUpdated;
	FZonActivityStopped OnActivityStopped;
	FZonActivityStopped OnActivityEnding;
	
	FZOnDeath OnDeath;

	FZOnCharacterEvent OnCharacterEvent;

	FZOnHealthChanged OnHealthChanged;
protected:
	void StopActivityLocal(EZCharacterActivity Activity);

	UFUNCTION()
	virtual void MeleeHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void Die(AController* DeathInstigator);

	void SetHealth(float InHealth, AController* DamageInstigator);

	void SetMaxHealth(float InMaxHealth);

	void ApplyKnockback(const FVector& ExplosionOrigin, float DamageScale);

	void EnterRagdoll();

	void ExitRagdoll();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Reliable_EnterRagdoll(const FVector& LaunchVelocity);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Reliable_ExitRagdoll(const FTransform& NewTransform);

	void GetUpFromRagdoll(bool bFacingUp);

	UFUNCTION()
	void FinishGettingUpFromRagdoll(UAnimMontage* Montage, bool bInterrupted);

	bool IsLocalPlayerControlled() const;

	virtual FGenericTeamId GetGenericTeamId() const;

	virtual void SetSleeping(bool bSleeping);

	void SetSitting(bool bSitting);

	void SetLooting(bool bLooting);

	bool SetInteractionTarget(TScriptInterface<IZInteractable> InInteractionTarget);

	void ClearInteractionTarget(EZCharacterActivity Activity);

	virtual void UpdateGaitState();

	EZGaitState GetEffectiveGaitState() const;

	EZGaitState GetMaxAllowedGaitState() const;

	void OnCanSprintChanged();

	void PlayActivityMontage(EZCharacterActivity Activity);
	void StopActivityMontage(EZCharacterActivity Activity);
	void OnEndMontageEnded(EZCharacterActivity Activity);
	void ForceStopActivityMontage(EZCharacterActivity Activity);
	bool EndsActivityInstantly(EZCharacterActivity Activity) const;
	virtual void StopActivity(EZCharacterActivity Activity);
	void ActivityEnding(EZCharacterActivity Activity);

	UPROPERTY(EditAnywhere)
	int32 Level;

	UPROPERTY(EditDefaultsOnly)
	EZFaction Faction;

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_MaxHealth, Category = "Health")
	float MaxHealth;

	UFUNCTION()
	void OnRep_MaxHealth();

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Health")
	float Health;

	UFUNCTION()
	void OnRep_Health();

	bool bDead;

	UPROPERTY(EditDefaultsOnly)
	float DamageHeadMultiplier;

	UPROPERTY(EditDefaultsOnly)
	float DamageLimbMultiplier;

	UPROPERTY(EditDefaultsOnly, Category = "Combat") // #ZTODO rethink this?
	bool bHasInfiniteAmmo;

	UPROPERTY(Replicated)
	bool bADSing;

	UPROPERTY(EditDefaultsOnly)
	float MeleeDamage;

	FTransform MeshOffset;
	FVector PelvicBoneOffset;

	UPROPERTY(EditDefaultsOnly)
	float KnockbackForce;

	UPROPERTY(EditDefaultsOnly)
	float KnockbackUpwardsBoost;

	UPROPERTY(EditDefaultsOnly)
	bool bCanRagdoll;

	bool bRagdolling;

	FTransform RagdollExitTransform;

	UPROPERTY(EditDefaultsOnly)
	float RagdollDuration;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> GetUpFromRagdollDown;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> GetUpFromRagdollUp;

	FTimerHandle TimerHandle_Ragdoll;

	UPROPERTY()
	TSet<AActor*> DamagedActors;

	FTimerHandle TimerHandle_GameplayEvent;

	UPROPERTY(EditDefaultsOnly)
	TSet<FName> HeadBoneNames;

	UPROPERTY(EditDefaultsOnly)
	TSet<FName> LimbBoneNames;

	UPROPERTY()
	TObjectPtr<AActor> LookAtTarget; // Used for aim offset

	bool bMirrorAnimation;

	UPROPERTY()
	TMap<EZCharacterActivity, TScriptInterface<IZInteractable>> InteractionTargets; // Map of interactable actors that the character is interacting with

	UPROPERTY(ReplicatedUsing = OnRep_InteractionTargetActors)
	TArray<FZActivityInteractionTarget> InteractionTargetActors;

	UFUNCTION()
	void OnRep_InteractionTargetActors();

	UPROPERTY(ReplicatedUsing = OnRep_CurrentActivities)
	EZCharacterActivity CurrentActivities;

	UFUNCTION()
	void OnRep_CurrentActivities(EZCharacterActivity OldActivities);

	UPROPERTY(ReplicatedUsing = OnRep_EndingActivities)
	EZCharacterActivity EndingActivities;

	UFUNCTION()
	void OnRep_EndingActivities(EZCharacterActivity OldEndingActivities);

	UPROPERTY(EditDefaultsOnly)
	FName EyesSocketName;

	EZGaitState DesiredGaitState;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UZCharacterAnimationData> AnimationData;

	static const FName NAME_MontageSectionStart;
	static const FName NAME_MontageSectionEnd;
	
	static const FName NAME_Pelvis;
	static const FName NAME_Hand_r_prop;
	static const FName NAME_Hand_l_prop;

	bool bMovementEnabled;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mover")
	bool bOrientRotationToMovement;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mover")
	bool bUseBaseRelativeMovement;
	
	bool bIsJumpJustPressed;
	bool bIsJumpPressed;
	bool bIsSlidePressed;
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> CapsuleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UZCharacterMoverComponent> MoverComponentBP;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UZInventoryComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UZEquipmentComponent> EquipmentComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UZStaminaComponent> StaminaComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UZSaveComponent> SaveComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> LeftHandPropMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> RightHandPropMesh;
};

