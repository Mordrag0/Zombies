// Copyright 2026 Luka Markuš. All rights reserved.


#include "Misc/ZLockpick.h"
#include "Components/StaticMeshComponent.h"

AZLockpick::AZLockpick()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	RootComponent = Mesh;
}

