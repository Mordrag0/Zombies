// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZInteractableBase.h"
#include "ZBed.generated.h"

class UStaticMeshComponent;
class AZCharacter;
enum class EZCharacterActivity : uint32;

UCLASS()
class ZOMBIES_API AZBed : public AZInteractableBase
{
	GENERATED_BODY()
	
public:	
	AZBed();

	bool GetRightSide() const { return bRightSide; }

	virtual TArray<UMeshComponent*> GetHighlightMeshes() const;
	virtual FTransform GetPivotPoint() const override;
	virtual EZCharacterActivity GetCharacterActivity() const override;

protected:
	UPROPERTY(EditInstanceOnly)
	bool bRightSide;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> LeftPivotPoint;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> RightPivotPoint;
};

