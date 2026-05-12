// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZInteractable.h"
#include "ZInventoryItem.generated.h"

class AZCharacter;
class UZInventoryItemData;

DECLARE_LOG_CATEGORY_EXTERN(LogZItem, Log, All);

UCLASS(Abstract)
class ZOMBIES_API AZInventoryItem : public AActor, public IZInteractable
{
	GENERATED_BODY()
	
public:	
	AZInventoryItem();
	
	virtual bool CanInteract(const AZCharacter* InCharacter) const override;
	virtual bool BeginInteract(AZCharacter* InCharacter) override;
	virtual void EndInteract(AZCharacter* InCharacter) override;
	virtual TArray<UMeshComponent*> GetHighlightMeshes() const override;
	virtual FTransform GetPivotPoint() const override;
	virtual EZCharacterActivity GetCharacterActivity() const override;

	virtual bool CanBeEquipped() const { return false; }
	virtual bool CanBeConsumed() const { return bCanBeConsumed; }

	virtual UMeshComponent* GetMesh() const { unimplemented(); return nullptr; }

	uint32 GetCount() const { return Count; }

    FPrimaryAssetId GetItemId() const;

	void StartPickUp(AZCharacter* InCharacter);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void SetOwner(AActor* NewOwner) override;

	AZCharacter* GetCharacterOwner() const { return CharacterOwner; }

protected:
    UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UZInventoryItemData> ItemData;

	virtual void BeginPlay() override;

	virtual void OnRep_Owner() override;

	UPROPERTY(EditDefaultsOnly)
	bool bCanBeConsumed;

	UPROPERTY(EditInstanceOnly)
	uint32 Count;

	UPROPERTY(Replicated)
	bool bPickedUp; // #ZTODO: do we need this and if so do we need it replicated?, we have InteractionTarget

	UPROPERTY()
	TScriptInterface<IZInteractable> InteractionTarget;

	UPROPERTY()
	TObjectPtr<AZCharacter> CharacterOwner;

private:
	virtual bool SetInteractionTarget(TScriptInterface<IZInteractable> InInteractionTarget);
};

