// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/ZCharacter.h"
#include "GameplayTagContainer.h"
#include "ZDelegateConnection.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "ZFPCharacter.generated.h"

class UZGameUserSettings;
struct FZInteractionStateArray;
class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;
class IZInteractable;
class USkeletalMeshComponent;
class AZNPCharacter;
class AZCharacter;

USTRUCT()
struct FZInteractionStateEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY()
	FZInteractionState InteractionState;
	
    void PostReplicatedAdd(const FZInteractionStateArray& InArraySerializer);
    void PostReplicatedChange(const FZInteractionStateArray& InArraySerializer);
    void PreReplicatedRemove(const FZInteractionStateArray& InArraySerializer);
};

USTRUCT()
struct FZInteractionStateArray : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FZInteractionStateEntry> InteractionStates;

	TWeakObjectPtr<AZFPCharacter> CharacterOwner;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize(InteractionStates, DeltaParams, *this);
	}
};

template<>
struct TStructOpsTypeTraits<FZInteractionStateArray> : public TStructOpsTypeTraitsBase2<FZInteractionStateArray>
{
    enum
    {
        WithNetDeltaSerializer = true,
    };
};

/**
 * 
 */
UCLASS()
class ZOMBIES_API AZFPCharacter : public AZCharacter
{
	GENERATED_BODY()

public:
	AZFPCharacter();
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void ProduceInput_Implementation(int32 SimTimeMs, FMoverInputCmdContext& InputCmdResult) override;
	
	virtual bool CanInteract(const AZCharacter* InCharacter) const override;
	virtual bool BeginInteract(AZCharacter* InCharacter) override;
	virtual bool ShouldHighlight(const AZFPCharacter* InCharacter) const override { return false; }
	virtual EZCharacterActivity GetCharacterActivity() const override;
	
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	virtual void Tick(float DeltaTime) override;

	virtual USkeletalMeshComponent* GetActualMesh() const override;

	virtual void SetADSing(bool bVal) override;
	
	void SetFOVSensAdjustment(float Val);

	virtual void OnCrouchUpdate(float RelativeLocZ) override;

	virtual bool HasSkill(EZSkill Skill) const override;

	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void StartInteractionActivity(const FZInteractionState& InInteractionState) override;
	virtual void UpdateActivity(const FZInteractionState& InInteractionState) override;

	virtual bool StartInteractingWith(TScriptInterface<IZInteractable> InInteractionTarget) override;
	virtual void StopInteractingWith(TScriptInterface<IZInteractable> InInteractionTarget, bool bForceInstant = false) override;

	UCameraComponent* GetActiveCameraComponent() const;
	
protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	virtual FGenericTeamId GetGenericTeamId() const override;
	
	virtual void StopActivity(EZCharacterActivity Activity) override;

	void MoveInput(const FInputActionValue& Value);
	void MoveInputCompleted(const FInputActionValue& Value);

	void LookInput(const FInputActionValue& Value);
	void LookInputCompleted(const FInputActionValue& Value);

	void MouseLookInput(const FInputActionValue& Value);
	
	void DoInteract();

	void DoSprintStart();

	void DoSprintEnd();

	void WalkStart();

	void WalkEnd();

	void DoOpenInventory();

	void DoOpenQuests();

	void DoOpenSkills();

	void DoOpenMap();

	void Escape();
	
	void ChangePerspective();
	void OnPerspectiveChanged();

	virtual void Die(AController* DeathInstigator) override;

	UFUNCTION(Server, Reliable)
	void Server_Reliable_StartInteractingWith(AActor* InInteractableActor);

	UFUNCTION(Client, Reliable)
	void Client_Reliable_StartInteractingWithFailed();

	UFUNCTION(Server, Reliable)
	void Server_Reliable_StopInteractingWith(AActor* InInteractableActor);

	UFUNCTION(Server, Reliable)
	void Server_Reliable_OnInteractionAttempted(AZNPCharacter* TargetNPC);

	void AddInteractionState(const FZInteractionState& InteractionState);
	void UpdateInteractionState(const FZInteractionState& InteractionState);
	void RemoveInteractionState(EZCharacterActivity Activity);

	virtual void UpdateGaitState() override;

	UFUNCTION(Server, Reliable)
	void Server_Reliable_SetADSing(bool bVal);
	
	void SwitchShoulder();
	
	void OnSwitchShoulder() const;
	
	float GetInteractionTraceDistance() const;
	
	void LoadGameUserSettings(const UZGameUserSettings& Settings);
	
	FZDelegateConnection SettingsConnection;

	float FOVSensAdjustment;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	float VerticalSensitivity;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	float HorizontalSensitivity;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	float VerticalMouseSensitivity;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	float HorizontalMouseSensitivity;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float FPPInteractionTraceDistance;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float TPPInteractionTraceDistance;

	UPROPERTY()
	TScriptInterface<IZInteractable> LookAtInteractionTarget; // Interactable actor that the player is looking at

	UPROPERTY()
	TScriptInterface<IZInteractable> PendingInteractionTarget;

	UPROPERTY(Replicated)
	FZInteractionStateArray InteractionStates;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MouseLookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> InventoryAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> CrouchAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> WalkAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> UnequipAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> OpenQuestsAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> OpenSkillsAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> OpenMapAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> EscapeAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> ChangePerspectiveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> SlideAction;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> SwitchShoulderAction;

	bool bWantsToWalk;
	bool bWantsToSprint;
	
	UPROPERTY(EditDefaultsOnly)
	bool bFirstPerson;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mover")
	bool bMaintainLastInputOrientation;
	
	FVector LastAffirmativeMoveInput;	// Movement input (intent or velocity) the last time we had one that wasn't zero
	
	FVector CachedMoveInputIntent;
	FVector CachedMoveInputVelocity;

	FRotator CachedLookInput;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UZCharacterAnimationData> FPAnimationData;
	
	bool bIsRightShoulder;
	
	UPROPERTY(EditDefaultsOnly)
	float RightShoulderOffset;

	UPROPERTY(EditDefaultsOnly)
	float LeftShoulderOffset;

private:
	/** Pawn mesh: first person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> FirstPersonMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FirstPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> ThirdPersonCameraComponent;
};

