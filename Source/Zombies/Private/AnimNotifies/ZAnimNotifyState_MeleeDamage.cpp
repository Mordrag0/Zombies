// Copyright 2026 Luka Markuš. All rights reserved.


#include "AnimNotifies/ZAnimNotifyState_MeleeDamage.h"
#include "Characters/ZCharacter.h"

FString UZAnimNotifyState_MeleeDamage::GetNotifyName_Implementation() const
{
	return TEXT("Melee Damage");
}

void UZAnimNotifyState_MeleeDamage::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (AZCharacter* Character = Cast<AZCharacter>(MeshComp->GetOwner()))
	{
		Character->BeginMeleeDamage(Type);
	}
}

void UZAnimNotifyState_MeleeDamage::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (AZCharacter* Character = Cast<AZCharacter>(MeshComp->GetOwner()))
	{
		Character->EndMeleeDamage(Type);
	}
}

