// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/ZAICharacter.h"
#include "ZZombieCharacter.generated.h"

class UZNavMoverComponent;
class USphereComponent;
class UAnimMontage;
enum class EZCharacterActivity : uint32;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FZOnAttackEnded);

UENUM(BlueprintType)
enum class EZZombieAttack : uint8
{
	None,
	Attack1,
};

/**
 * 
 */
UCLASS()
class ZOMBIES_API AZZombieCharacter : public AZAICharacter
{
	GENERATED_BODY()

public:
	AZZombieCharacter();
	
	virtual void BeginPlay() override;

	virtual void ProduceInput_Implementation(int32 SimTimeMs, FMoverInputCmdContext& InputCmdResult) override;
	
	virtual EZCharacterActivity GetCharacterActivity() const override;

	virtual bool CanInteract(const AZCharacter* InCharacter) const override;

	void Attack(AZCharacter* Target);

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	virtual void BeginMeleeDamage(EZZombieAttackType Type) override;
	virtual void EndMeleeDamage(EZZombieAttackType Type) override;
	
	FZOnAttackEnded OnAttackEnded;

protected:
	UPROPERTY(EditDefaultsOnly)
	TMap<EZZombieAttack, TObjectPtr<UAnimMontage>> Attacks;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> LeftHand;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> RightHand;
};

