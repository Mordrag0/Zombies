// Copyright 2026 Luka Markuš. All rights reserved.


#include "ZNetworkTypes.h"
#include "Inventory/ZInventoryComponent.h"

bool FZInteractionState::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	// Serialize InteractionTargetActor and Activity normally
	Ar << InteractionTargetActor;
	Ar << Activity;

	// Manually serialize which variant type is active and its data
	if (Ar.IsSaving())
	{
		uint8 TypeIndex = Params.GetIndex();
		Ar << TypeIndex;
		// Serialize based on active type
		switch (TypeIndex)
		{
			case 0: // FZNoInteractionParams
				break;
			case 1: // FZLootingParams
				break;
			case 2: // FZNPCDialogueParams
			{
				FZNPCDialogueParams& DP = Params.Get<FZNPCDialogueParams>();
				Ar << DP.PendingDialogueID;
				Ar << DP.DialogueContext;
				break;
			}
			case 3: // FZDialogueParams
			{
				FZDialogueParams& DP = Params.Get<FZDialogueParams>();
				Ar << DP.bDialogueLocked;
				Ar << DP.DialogueContext;
				Ar << DP.DialogueIDs;
				break;
			}
			default:
				ensure(0);
				break;
		}
	}
	else
	{
		uint8 TypeIndex;
		Ar << TypeIndex;
		switch (TypeIndex)
		{
			case 0: // FZNoInteractionParams
				Params.Set<FZNoInteractionParams>(FZNoInteractionParams{});
				break;
			case 1: // FZLootingParams
				if (InteractionTargetActor)
				{
					Params.Set<FZLootingParams>(FZLootingParams(InteractionTargetActor->FindComponentByClass<UZInventoryComponent>()));
				}
				break;
			case 2: // FZNPCDialogueParams
			{
				FGameplayTag PendingDialogueID;
				Ar << PendingDialogueID;
				EZDialogueContext DialogueContext;
				Ar << DialogueContext;
				Params.Set<FZNPCDialogueParams>(FZNPCDialogueParams(PendingDialogueID, DialogueContext));
				break;
			}
			case 3: // FZDialogueParams
			{
				bool bDialogueLocked;
				Ar << bDialogueLocked;
				EZDialogueContext DialogueContext;
				Ar << DialogueContext;
				TArray<FGameplayTag> DialogueIDs;
				Ar << DialogueIDs;
				Params.Set<FZDialogueParams>(FZDialogueParams(bDialogueLocked, DialogueContext, DialogueIDs));
				break;
			}
			default:
				ensure(0);
				break;
		}
	}
	bOutSuccess = true;
	return true;
}

