// Copyright 2026 Luka Markuš. All rights reserved.


#include "Inventory/ZSimpleInventoryItem.h"
#include "Components/StaticMeshComponent.h"

AZSimpleInventoryItem::AZSimpleInventoryItem()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	RootComponent = Mesh;
}

