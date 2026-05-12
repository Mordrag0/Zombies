// Copyright 2026 Luka Markuš. All rights reserved.


#include "Misc/ZBed.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Characters/ZCharacter.h"
#include "ZTypes.h"

AZBed::AZBed()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	RootComponent = Mesh;

	RightPivotPoint = CreateDefaultSubobject<USceneComponent>("Right pivot Point");
	RightPivotPoint->SetupAttachment(Mesh);

	LeftPivotPoint = CreateDefaultSubobject<USceneComponent>("Left pivot Point");
	LeftPivotPoint->SetupAttachment(Mesh);

	bRightSide = true;
}

TArray<UMeshComponent*> AZBed::GetHighlightMeshes() const
{
	return { Mesh };
}

FTransform AZBed::GetPivotPoint() const
{
	return bRightSide ? RightPivotPoint->GetComponentTransform() : LeftPivotPoint->GetComponentTransform();
}

EZCharacterActivity AZBed::GetCharacterActivity() const
{
	return EZCharacterActivity::Sleeping;
}


