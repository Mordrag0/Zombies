// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZLock.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UCameraComponent;

UCLASS()
class ZOMBIES_API AZLock : public AActor
{
	GENERATED_BODY()
	
public:	
	AZLock();

	void StartLockpicking();

	void StopLockpicking();

	void RotateLockpick(float X);

	void TensionStart();

	void TensionEnd();

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;

	void UpdateCylinderRotation();

	float GetMaxCylinderRotationAmount() const;

	void OnUnlocked();

	void BreakLockpick();

	void ResetLockpick();

	void Resume();

	float CurrentLockpickAngle;

	bool bApplyingTension;

	UPROPERTY(EditDefaultsOnly)
	float MinLockpickRotation;

	UPROPERTY(EditDefaultsOnly)
	float MaxLockpickRotation;

	float TargetLockpickAngle;

	float AcceptableDifference;
	
	UPROPERTY(EditDefaultsOnly)
	float MinCylinderRotation;

	UPROPERTY(EditDefaultsOnly)
	float MaxCylinderRotation;

	UPROPERTY(EditDefaultsOnly)
	float BindingRange;

	float CurrentCylinderRotation;

	UPROPERTY(EditDefaultsOnly)
	float CylinderRotationSpeed;

	UPROPERTY(EditDefaultsOnly)
	float TimeToBreakLockpick;

	float TensionTimeAtMax;

	bool bPaused;

	FTimerHandle TimerHandle_Break;

	UPROPERTY(EditDefaultsOnly)
	float BreakResetDuration;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> OuterLockMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> InnerLockMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> LockpickMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> ScrewdriverMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> LockpickingCamera;
};

