// Copyright 2026 Luka Markuš. All rights reserved.


#include "Misc/ZChair.h"
#include "Characters/ZCharacter.h"
#include "ZTypes.h"

AZChair::AZChair()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	RootComponent = Mesh;

	PivotPoint = CreateDefaultSubobject<USceneComponent>("Pivot Point");
	PivotPoint->SetupAttachment(Mesh);
}

TArray<UMeshComponent*> AZChair::GetHighlightMeshes() const
{
	return { Mesh };
}

FTransform AZChair::GetPivotPoint() const
{
	return PivotPoint->GetComponentTransform();
}

EZCharacterActivity AZChair::GetCharacterActivity() const
{
	return EZCharacterActivity::Sitting;
}


