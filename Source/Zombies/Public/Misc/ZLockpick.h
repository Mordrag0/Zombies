// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/ZInventoryItem.h"
#include "ZLockpick.generated.h"

class UStaticMeshComponent;

/**
 * 
 */
UCLASS()
class ZOMBIES_API AZLockpick : public AZInventoryItem
{
	GENERATED_BODY()
	
public:
	AZLockpick();
	
	virtual UMeshComponent* GetMesh() const override { return Mesh; }

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Mesh;
};

