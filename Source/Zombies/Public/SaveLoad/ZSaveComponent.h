// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ZSaveComponent.generated.h"


UCLASS( ClassGroup=(Custom) )
class ZOMBIES_API UZSaveComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UZSaveComponent();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	FGuid GetGUID() const { return ActorGUID; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	FGuid ActorGUID; // #ZTODO
};

