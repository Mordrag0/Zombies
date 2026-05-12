// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ZStaminaComponent.generated.h"

class AZCharacter;
class UZCharacterMovementComponent;

DECLARE_MULTICAST_DELEGATE_TwoParams(FZOnStaminaChanged, float, float);
DECLARE_DELEGATE(FZOnCanSprintChanged)

UCLASS( ClassGroup=(Custom) )
class ZOMBIES_API UZStaminaComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UZStaminaComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool IsExhausted() const { return bExhausted; }

	float GetStamina() const { return Stamina; }

	float GetMaxStamina() const { return MaxStamina; }

	bool GetServerAllowsSprint() const { return bServerAllowsSprint; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	bool CanSprint() const { return bServerAllowsSprint && !bExhausted; }

	FZOnStaminaChanged OnStaminaChanged;

	FZOnCanSprintChanged OnCanSprintChanged;

protected:
	virtual void BeginPlay() override;

	void SetStamina(float InStamina);
	
	void OnExhaustedStart();

	UFUNCTION()
	void OnExhaustedEnd();

	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	float MaxStamina;

	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	float SprintDrainRate;

	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	float WalkRegenRate;

	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	float IdleRegenRate;

	UPROPERTY(EditDefaultsOnly, Category = "Stamina")
	float ExhaustedTime;

	UPROPERTY(ReplicatedUsing = OnRep_Stamina)
	float Stamina;

	UFUNCTION()
	void OnRep_Stamina();
	
	UPROPERTY(ReplicatedUsing = OnRep_ServerAllowsSprint)
	bool bServerAllowsSprint;

	UFUNCTION()
	void OnRep_ServerAllowsSprint();

	float PredictedStamina;

	bool bExhausted;

	FTimerHandle TimerHandle_Exhausted;

	UPROPERTY()
	TObjectPtr<AZCharacter> CharacterOwner;

	UPROPERTY()
	TObjectPtr<UZCharacterMovementComponent> CharacterMovement;
};

