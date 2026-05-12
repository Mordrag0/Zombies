// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "ZAnimNotify_GameplayEvent.generated.h"

enum class EZGameplayEvent : uint8;

/**
 * 
 */
UCLASS()
class ZOMBIES_API UZAnimNotify_GameplayEvent : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	FString GetNotifyName_Implementation() const;

	void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference);

protected:
	UPROPERTY(EditAnywhere)
	EZGameplayEvent Event;
};

