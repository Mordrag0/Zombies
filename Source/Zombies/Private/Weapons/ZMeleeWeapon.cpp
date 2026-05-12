// Copyright 2026 Luka Markuš. All rights reserved.


#include "Weapons/ZMeleeWeapon.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ZGameMode.h"
#include "Components/SceneComponent.h"
#include "ZTypes.h"

AZMeleeWeapon::AZMeleeWeapon()
{
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(Root);

	CollisionComp = CreateDefaultSubobject<UBoxComponent>("Collision Comp");
	CollisionComp->SetupAttachment(Mesh);

	EquipType = EZEquipType::Melee;
}

