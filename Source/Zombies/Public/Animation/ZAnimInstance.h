// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ZAnimInstance.generated.h"

class UZCharacterMoverComponent;
class AZCharacter;
enum class EZCharacterActivity : uint32;
enum class EZCharacterActivityBP : uint8;

/**
 * 
 */
UCLASS()
class ZOMBIES_API UZAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UZAnimInstance();

	virtual void NativeInitializeAnimation();

	virtual void NativeUpdateAnimation(float DeltaSeconds);

	void SaveRagdollPoseSnapShot();

	void OnExitRagdoll(const FTransform& InTransform);

	void OnEndRagdollSnapShot();

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	bool IsPerformingActivityBP(EZCharacterActivityBP Activity) const;

	bool IsPerformingActivity(EZCharacterActivity Activity) const { return EnumHasAnyFlags(CurrentActivities, Activity); }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float WalkForward;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float WalkRight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bDead;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bMirror;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bRagdollPose;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bGettingUp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RagdollPoseDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName RagdollPoseName;

	UPROPERTY(BlueprintReadOnly)
	FTransform SnapshotRelativeTransform;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EZCharacterActivity CurrentActivities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float LookUp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float LookRight;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AZCharacter> CharacterOwner;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UZCharacterMoverComponent> CharacterMover;
};

