// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZInteractable.h"
#include "ZLockableBase.h"
#include "ZContainer.generated.h"

class AZCharacter;
class UZInventoryComponent;
class USceneComponent;
class USkeletalMeshComponent;

UCLASS()
class ZOMBIES_API AZContainer : public AZLockableBase
{
	GENERATED_BODY()
	
public:	
	AZContainer();

	virtual bool BeginInteract(AZCharacter* InCharacter) override;
	virtual void EndInteract(AZCharacter* InCharacter) override;
	
	virtual TArray<UMeshComponent*> GetHighlightMeshes() const override { return { Mesh }; }
	virtual EZCharacterActivity GetCharacterActivity() const override;

	virtual FZInteractionParams GetInteractionParams(const AZCharacter* InCharacter) const override;

	virtual void Unlock() override;

protected:

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UZInventoryComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> Mesh;
};

