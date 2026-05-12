// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/ZInventoryItem.h"
#include "ZCoins.generated.h"

class UStaticMeshComponent;

/**
 * 
 */
UCLASS()
class ZOMBIES_API AZCoins : public AZInventoryItem
{
	GENERATED_BODY()
	
public:
	AZCoins();

	virtual UMeshComponent* GetMesh() const override { return Mesh; }

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;
};

