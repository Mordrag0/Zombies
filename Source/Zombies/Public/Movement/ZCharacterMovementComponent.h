// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ZCharacterMovementComponent.generated.h"

class UZStaminaComponent;
class AZCharacter;
enum class EZGaitState : uint8;

class FZCharacterNetworkMoveData : public FCharacterNetworkMoveData
{
public:
	using Super = FCharacterNetworkMoveData;

	FZCharacterNetworkMoveData();

    EZGaitState GaitState;
    float CrouchAmount;

    virtual void ClientFillNetworkMoveData(const FSavedMove_Character& ClientMove, ENetworkMoveType MoveType) override;
    virtual bool Serialize(UCharacterMovementComponent& CharacterMovement, FArchive& Ar, UPackageMap* PackageMap, ENetworkMoveType MoveType) override;
};

class FZCharacterNetworkMoveDataContainer : public FCharacterNetworkMoveDataContainer
{
public:
    FZCharacterNetworkMoveDataContainer();

    FZCharacterNetworkMoveData MoveData[3];
};

/** FSavedMove_Character represents a saved move on the client that has been sent to the server and might need to be played back. */
class FSavedMove_ZCharacter : public FSavedMove_Character
{
public:
	using Super = FSavedMove_Character;

	// Resets all saved variables.
	virtual void Clear() override;

	// This is used to check whether or not two moves can be combined into one.
	// Basically you just check to make sure that the saved variables are the same.
	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const override;

	virtual void CombineWith(const FSavedMove_Character* OldMove, ACharacter* InCharacter, APlayerController* PC, const FVector& OldStartLocation) override;

	// Sets up the move before sending it to the server. 
	virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;
	// Sets variables on character movement component before making a predictive correction.
	virtual void PrepMoveFor(class ACharacter* Character) override;

	virtual void SetInitialPosition(ACharacter* Character) override;

	virtual void PostUpdate(ACharacter* Character, EPostUpdateMode PostUpdateMode) override;

	EZGaitState SavedGaitState;
	float CrouchAmount;
};

// Network data representation on the client
class FNetworkPredictionData_Client_ZCharacter : public FNetworkPredictionData_Client_Character
{
public:
	FNetworkPredictionData_Client_ZCharacter(const UCharacterMovementComponent& ClientMovement);

	using Super = FNetworkPredictionData_Client_Character;

	// Allocates a new copy of our custom saved move

	virtual FSavedMovePtr AllocateNewMove() override
	{
		return FSavedMovePtr(new FSavedMove_ZCharacter());
	}
};

/**
 * 
 */
UCLASS()
class ZOMBIES_API UZCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	friend class FSavedMove_ZCharacter;
	
public:
	UZCharacterMovementComponent();

	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	virtual void BeginPlay() override;

	virtual float GetMaxSpeed() const override;

	bool IsMovingForward() const;

	bool IsMoving() const;
	
	virtual void Crouch(bool bClientSimulation = false) override;
	virtual void UnCrouch(bool bClientSimulation = false) override;
	virtual bool IsCrouching() const override;

	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;

	virtual void UpdateCharacterStateAfterMovement(float DeltaSeconds) override;

	float GetCrouchAmount() const { return CrouchAmount; }

	void SetGaitState(EZGaitState InGaitState);

	EZGaitState GetGaitState() const { return GaitState; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void SimulateMovement(float DeltaTime) override;

	virtual void PerformMovement(float DeltaTime) override;

	void UpdateCrouch(float DeltaTime);

	bool CrouchResize(float NewCrouchAmount, float DeltaCrouch, bool bClientSimulation = false);

	void UpdateGaitState();

	virtual void MoveAutonomous(float ClientTimeStamp, float DeltaTime, uint8 CompressedFlags, const FVector& NewAccel) override;

	float CrouchAmount;

	UPROPERTY(Replicated)
	EZGaitState GaitState;

	EZGaitState DesiredGaitState;

	UPROPERTY(EditDefaultsOnly)
	float MaxSprintSpeed;
	
	UPROPERTY(EditDefaultsOnly)
	float MaxRunSpeed;
	
	/** Time to crouch on ground in seconds */
	UPROPERTY(EditDefaultsOnly, Meta = (ClampMin = "0", UIMin = "0"))
	float CrouchTime;

	/** Time to crouch in air in seconds */
	UPROPERTY(EditDefaultsOnly, Meta = (ClampMin = "0", UIMin = "0"))
	float CrouchJumpTime;

	float CrouchTimeInv;
	float CrouchJumpTimeInv;
	
	float DefaultCapsuleHalfHeight;
	float DefaultMeshRelativeZ;
	float DefaultCameraRelativeZ;
	
	FZCharacterNetworkMoveDataContainer MoveDataContainer;

	UPROPERTY()
	TObjectPtr<AZCharacter> ZCharacterOwner;

	UPROPERTY()
	TObjectPtr<UZStaminaComponent> StaminaComponent;
};

