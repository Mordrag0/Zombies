// Copyright 2026 Luka Markuš. All rights reserved.


#include "Characters/ZZombieCharacter.h"
#include "Components/SphereComponent.h"
#include "Animation/ZZombieAnimInstance.h"
#include "Animation/AnimMontage.h"
#include "ZTypes.h"
#include "Components/SkeletalMeshComponent.h"
#include "Movement/ZNavMoverComponent.h"

AZZombieCharacter::AZZombieCharacter() 
{
	LeftHand = CreateDefaultSubobject<USphereComponent>(TEXT("Lefthand"));
	LeftHand->SetupAttachment(GetMesh(), FName("hand_l"));

	RightHand = CreateDefaultSubobject<USphereComponent>(TEXT("Righthand"));
	RightHand->SetupAttachment(GetMesh(), FName("hand_r"));

	Faction = EZFaction::Zombies;
}

EZCharacterActivity AZZombieCharacter::GetCharacterActivity() const
{
	return bDead ? EZCharacterActivity::Looting : EZCharacterActivity::None;
}

bool AZZombieCharacter::CanInteract(const AZCharacter* InCharacter) const
{
	return bDead && Super::CanInteract(InCharacter); // Can only interact with a dead zombie
}

void AZZombieCharacter::Attack(AZCharacter* Target)
{
	EZZombieAttack AttackType = EZZombieAttack::Attack1;
	DamagedActors.Empty();

	if (UZZombieAnimInstance* ZombieAnimInstance = Cast<UZZombieAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		if (Attacks.Contains(AttackType))
		{
			if (UAnimMontage* AttackMontage = Attacks[AttackType])
			{
				float Duration = ZombieAnimInstance->Montage_Play(AttackMontage);
				if (Duration > 0.f)
				{
					FOnMontageEnded EndDelegate;
					EndDelegate.BindUObject(this, &ThisClass::OnAttackMontageEnded);

					ZombieAnimInstance->Montage_SetEndDelegate(EndDelegate, AttackMontage);
				}
			}
		}
	}
}

void AZZombieCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	OnAttackEnded.Broadcast();
}

void AZZombieCharacter::BeginMeleeDamage(EZZombieAttackType Type)
{
	if (!HasAuthority())
	{
		return;
	}
	if ((Type == EZZombieAttackType::BothHands) || (Type == EZZombieAttackType::LeftHand))
	{
		LeftHand->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	if ((Type == EZZombieAttackType::BothHands) || (Type == EZZombieAttackType::RightHand))
	{
		RightHand->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

void AZZombieCharacter::EndMeleeDamage(EZZombieAttackType Type)
{
	if (!HasAuthority())
	{
		return;
	}
	if ((Type == EZZombieAttackType::BothHands) || (Type == EZZombieAttackType::LeftHand))
	{
		LeftHand->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if ((Type == EZZombieAttackType::BothHands) || (Type == EZZombieAttackType::RightHand))
	{
		RightHand->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AZZombieCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	LeftHand->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::MeleeHit);
	RightHand->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::MeleeHit);
}

void AZZombieCharacter::ProduceInput_Implementation(int32 SimTimeMs, FMoverInputCmdContext& InputCmdResult)
{
	Super::ProduceInput_Implementation(SimTimeMs, InputCmdResult);
}

