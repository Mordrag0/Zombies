// Copyright 2026 Luka Markuš. All rights reserved.


#include "SaveLoad/ZSaveSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "SaveLoad/ZSaveGame.h"
#include "SaveLoad/ZSaveComponent.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "ZGameState.h"
#include "ZLog.h"
#include "Characters/ZCharacter.h"
#include "Serialization/MemoryWriter.h"

void UZSaveSubsystem::SaveGame(const FString& SlotName)
{
	CleanInvalidPointers();
	UZSaveGame* SaveGameObject = Cast<UZSaveGame>(UGameplayStatics::CreateSaveGameObject(UZSaveGame::StaticClass()));

	SaveGameState(SaveGameObject);

	for (const TWeakObjectPtr<UZSaveComponent> SaveComponent : RegisteredSaveComponents)
	{
		if(!SaveComponent.IsValid())
		{
			continue;
		}
		AActor* Actor = SaveComponent->GetOwner();
		FZActorSaveData ActorData;
		ActorData.ActorGUID = SaveComponent->GetGUID();
		ActorData.Name = Actor->GetName();
		ActorData.Transform = Actor->GetActorTransform();

		FMemoryWriter MemoryWriter(ActorData.ByteData);

		FObjectAndNameAsStringProxyArchive Archive(MemoryWriter, true);
		Archive.ArIsSaveGame = true;

		Archive.UsingCustomVersion(FZSaveWorldVersion::GUID);
		Archive.UsingCustomVersion(AZCharacter::FZSaveVersion::GUID); // #ZTODO What's the point of this GUID?

		Actor->Serialize(Archive);

		SaveGameObject->SavedActors.Add(ActorData);
	}

	UGameplayStatics::SaveGameToSlot(SaveGameObject, SlotName, 0);
}

void UZSaveSubsystem::LoadGame(const FString& SlotName)
{
	CleanInvalidPointers();
	if (!UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		UE_LOG(LogZombies, Error, TEXT("Trying to load a save game that does not exist: %s"), *SlotName);
		return;
	}

	UZSaveGame* SaveGameObject = Cast<UZSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
	if (!ensure(SaveGameObject))
	{
		return;
	}

	LoadGameState(SaveGameObject);
	
	// Build lookup map
	TMap<FString, FZActorSaveData> SaveMap;

	for (const FZActorSaveData& Data : SaveGameObject->SavedActors)
	{
		SaveMap.Add(Data.Name, Data);
	}
	
	for (TWeakObjectPtr<UZSaveComponent> CompPtr : RegisteredSaveComponents)
	{
		UZSaveComponent* SaveComponent = CompPtr.Get();
		if (!SaveComponent)
		{
			continue;
		}

		AActor* Actor = SaveComponent->GetOwner();
		if (!Actor)
		{
			continue;
		}

		FZActorSaveData* FoundData = SaveMap.Find(Actor->GetName());
		if (!FoundData)
		{
			continue;
		}

		Actor->SetActorTransform(FoundData->Transform);

		FMemoryReader MemoryReader(FoundData->ByteData);

		FObjectAndNameAsStringProxyArchive Archive(MemoryReader, true);
		Archive.ArIsSaveGame = true;

		Archive.UsingCustomVersion(FZSaveWorldVersion::GUID);
		Archive.UsingCustomVersion(AZCharacter::FZSaveVersion::GUID);

		Actor->Serialize(Archive);
	}
}

void UZSaveSubsystem::RegisterComponent(UZSaveComponent* SaveComponent)
{
	RegisteredSaveComponents.Add(SaveComponent);
}

void UZSaveSubsystem::UnregisterComponent(UZSaveComponent* SaveComponent)
{
	RegisteredSaveComponents.Remove(SaveComponent);
}

void UZSaveSubsystem::SaveGameState(UZSaveGame* Save)
{
	if (AZGameState* GS = GetWorld()->GetGameState<AZGameState>())
	{
		Save->CompletedEvents = GS->CompletedEvents;
		Save->AvailableDialogueEvents = GS->AvailableEvents;
		Save->CompletedQuests = GS->CompletedQuests;
		Save->OpenedQuests = GS->OpenedQuests;
		Save->FailedQuests = GS->FailedQuests;
		Save->UnavailableQuests = GS->UnavailableQuests;
		Save->FactionReputation = GS->FactionReputation;
	}
}

void UZSaveSubsystem::LoadGameState(UZSaveGame* Save)
{
	if (AZGameState* GS = GetWorld()->GetGameState<AZGameState>())
	{
		GS->CompletedEvents = Save->CompletedEvents;
		GS->AvailableEvents = Save->AvailableDialogueEvents;
		GS->CompletedQuests = Save->CompletedQuests;
		GS->OpenedQuests = Save->OpenedQuests;
		GS->FailedQuests = Save->FailedQuests;
		GS->UnavailableQuests = Save->UnavailableQuests;
		GS->FactionReputation = Save->FactionReputation;
	}
}

void UZSaveSubsystem::CleanInvalidPointers()
{
	RegisteredSaveComponents.RemoveAll([](TWeakObjectPtr<UZSaveComponent> Ptr)
	{
		return !Ptr.IsValid();
	});
}

