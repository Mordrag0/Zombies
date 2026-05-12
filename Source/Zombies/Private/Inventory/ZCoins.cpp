// Copyright 2026 Luka Markuš. All rights reserved.


#include "Inventory/ZCoins.h"
#include "Characters/ZCharacter.h"
#include "Inventory/ZInventoryComponent.h"

AZCoins::AZCoins()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
}


