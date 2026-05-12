// Copyright 2026 Luka Markuš. All rights reserved.


#include "AnimNotifies/ZAnimNotify_GameplayEvent.h"
#include "ZLog.h"
#include "Characters/ZCharacter.h"
#include "Components/ZEquipmentComponent.h"

FString UZAnimNotify_GameplayEvent::GetNotifyName_Implementation() const
{
	return TEXT("Gameplay Event");
}

void UZAnimNotify_GameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (AZCharacter* Character = MeshComp->GetOwner<AZCharacter>())
	{
		if (Character->IsLocallyControlled() && Character->GetActualMesh() == MeshComp)
		{
			//Character->GetEquipmentComponent()->HandleGameplayEvent(Event);
		}
	}
	else
	{
		UE_LOG(LogZombies, Error, TEXT("UZAnimNotify_GameplayEvent::Notify Couldn't get ZCharacter"));
	}
}
