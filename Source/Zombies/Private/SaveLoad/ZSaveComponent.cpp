// Copyright 2026 Luka Markuš. All rights reserved.


#include "SaveLoad/ZSaveComponent.h"
#include "SaveLoad/ZSaveSubsystem.h"

UZSaveComponent::UZSaveComponent()
{
}


void UZSaveComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UZSaveSubsystem* SaveSystem = GetWorld()->GetGameInstance()->GetSubsystem<UZSaveSubsystem>())
	{
		SaveSystem->UnregisterComponent(this);
	}

	Super::EndPlay(EndPlayReason);
}

void UZSaveComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!ActorGUID.IsValid())
	{
		ActorGUID = FGuid::NewGuid();
	}
	
	if (UZSaveSubsystem* SaveSystem = GetWorld()->GetGameInstance()->GetSubsystem<UZSaveSubsystem>())
	{
		SaveSystem->RegisterComponent(this);
	}
}


