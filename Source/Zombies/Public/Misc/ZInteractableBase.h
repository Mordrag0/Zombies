// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZInteractable.h"
#include "ZInteractableBase.generated.h"

class AZInteractableBase;

UCLASS(Abstract)
class ZOMBIES_API AZInteractableBase : public AActor, public IZInteractable
{
	GENERATED_BODY()
	
public:	
	AZInteractableBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual bool CanInteract(const AZCharacter* InCharacter) const override;
	virtual bool BeginInteract(AZCharacter* InCharacter) override;
	virtual void EndInteract(AZCharacter* InCharacter) override;
	
	virtual TArray<UMeshComponent*> GetHighlightMeshes() const { unimplemented(); return {}; }
	virtual FTransform GetPivotPoint() const { unimplemented(); return FTransform(); }
	virtual EZCharacterActivity GetCharacterActivity() const;

	AZCharacter* GetInteractingCharater() const { return InteractingCharacter; }

protected:
	virtual bool SetInteractionTarget(TScriptInterface<IZInteractable> InInteractionTarget);
	virtual void ClearInteractionTarget(TScriptInterface<IZInteractable> InInteractionTarget);

	TScriptInterface<IZInteractable> InteractionTarget;

	UPROPERTY(ReplicatedUsing = OnRep_InteractingCharacter)
	TObjectPtr<AZCharacter> InteractingCharacter;

	UFUNCTION()
	void OnRep_InteractingCharacter();
};

