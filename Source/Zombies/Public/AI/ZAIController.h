// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GenericTeamAgentInterface.h"
#include "GameplayTagContainer.h"
#include "ZDelegateConnection.h"
#include "StructUtils/StructView.h"
#include "ZAIController.generated.h"

class AZCharacter;
class UStateTreeAIComponent;
struct FInstancedStruct;

/**
 * 
 */
UCLASS()
class ZOMBIES_API AZAIController : public AAIController
{
	GENERATED_BODY()

public:
	AZAIController();

	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;

	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

	AZCharacter* GetZCharacter() const { return ZCharacter; }

	void SetCombatTarget(AZCharacter* CharacterTarget);

	AZCharacter* GetCombatTarget() const;

	void SendStateTreeEvent(FGameplayTag EventTag);

	void SendStateTreeEvent(FGameplayTag EventTag, const FInstancedStruct& Payload);

protected:
	virtual void BeginPlay() override;

	bool ShouldAttack(AActor* Other) const;

	bool ShouldLookAt(AActor* Other) const;

	void OnCombatTargetDied(AZCharacter* InCharacter);

	void SetLookAtTarget(AActor* Actor);
	
	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;

	virtual void OnDeath();

	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION()
	void OnTargetPerceptionForgotten(AActor* Actor);

	virtual void OnSensedLookAtActorAdded(AActor* SensedActor) {}

	virtual void OnSensedLookAtActorRemoved(AActor* SensedActor) {}

	void OnCharacterEvent(FGameplayTag EventTag);

	virtual void PreSendStateTreeEvent(FGameplayTag EventTag, const FConstStructView Payload = FConstStructView()) {}

	UPROPERTY()
	TSet<TObjectPtr<AZCharacter>> SensedTargetActors;

	UPROPERTY()
	TSet<TObjectPtr<AActor>> SensedLookAtActors;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AZCharacter> CombatTarget;

	FZDelegateConnection OnCombatTargetDeathConnection;
	FZDelegateConnection OnDeathConnection;
	FZDelegateConnection OnCharacterEventConnection;

private:
	UPROPERTY()
	TObjectPtr<AZCharacter> ZCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStateTreeAIComponent> StateTreeComponent;
};

