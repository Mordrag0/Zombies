// Copyright 2026 Luka Markuš. All rights reserved.


#include "AnimNotifies/ZAnimNotify_DisableRagdoll.h"
#include "Characters/ZCharacter.h"
#include "Animation/ZAnimInstance.h"
#include "ZLog.h"

FString UZAnimNotify_DisableRagdoll::GetNotifyName_Implementation() const
{
	return TEXT("Disable ragdoll");
}

void UZAnimNotify_DisableRagdoll::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (AZCharacter* CharacterOwner = MeshComp->GetOwner<AZCharacter>())
	{
		if (UZAnimInstance* AnimInst = Cast<UZAnimInstance>(CharacterOwner->GetMesh()->GetAnimInstance()))
		{
			AnimInst->OnEndRagdollSnapShot();
		}
		else
		{
			UE_LOG(LogZombies, Error, TEXT("UZAnimNotify_DisableRagdoll::Notify Couldn't get ZAnimInstance"));
		}
	}
	else
	{
		UE_LOG(LogZombies, Error, TEXT("UZAnimNotify_DisableRagdoll::Notify Couldn't get ZCharacter"));
	}
}

