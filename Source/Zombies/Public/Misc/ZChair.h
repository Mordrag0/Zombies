// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZInteractableBase.h"
#include "ZChair.generated.h"

class AZCharacter;
enum class EZCharacterActivity : uint32;

UCLASS()
class ZOMBIES_API AZChair : public AZInteractableBase
{
	GENERATED_BODY()
	
public:	
	AZChair();

	virtual TArray<UMeshComponent*> GetHighlightMeshes() const override;
	virtual FTransform GetPivotPoint() const override;
	virtual EZCharacterActivity GetCharacterActivity() const override;

protected:

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> PivotPoint;
};

