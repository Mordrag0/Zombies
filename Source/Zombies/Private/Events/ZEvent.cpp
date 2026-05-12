// Copyright 2026 Luka Markuš. All rights reserved.


#include "Events/ZEvent.h"
#include "ZGameState.h"
#include "ZTypes.h"
#include "Characters/ZFPCharacter.h"
#include "Characters/ZNPCharacter.h"
#include "ZPlayerController.h"
#include "AI/ZNPCAIController.h"
#include "Components/ZLevelingComponent.h"
#include "Components/ZSkillsComponent.h"
#include "Inventory/ZInventoryComponent.h"
#include "Player/ZPlayerState.h"

bool FZEventCondition_Reputation::IsMet(const AZPlayerController* PC) const
{
	if (const AZGameState* GS = PC ? PC->GetWorld()->GetGameState<AZGameState>() : nullptr)
	{
		const float Reputation = GS->GetReputation(Faction);
		if (bOver)
		{
			return Reputation >= Threshold;
		}
		else
		{
			return  Reputation <= Threshold;
		}
	}
	return false;
}

FString FZEventCondition_Reputation::ToString() const
{
	const FString Operator = bOver ? TEXT(">") : TEXT("<");
	return FString::Printf(TEXT("Reputation with %s %s %f"), *ENUM_TO_STRING(EZFaction, Faction), *Operator, Threshold);
}

bool FZEventCondition_Inventory::IsMet(const AZPlayerController* PC) const
{
	if (const AZFPCharacter* Character = PC ? PC->GetFPCharacter() : nullptr)
	{
		if (ensure(ItemID.IsValid()))
		{
			return Character->GetInventoryComponent()->GetItemCount(ItemID) >= Count;
		}
	}
	return false;
}

FString FZEventCondition_Inventory::ToString() const
{
	const FString ItemString = ItemID.IsValid() ? ItemID.ToString() : TEXT("Invalid");
	if (Count != 1)
	{
		return FString::Printf(TEXT("Has %d of %s"), Count, *ItemString);
	}
	else
	{
		return FString::Printf(TEXT("Has %s"), *ItemString);
	}
}

bool FZEventCondition_Skill::IsMet(const AZPlayerController* PC) const
{
	if (AZPlayerState* PS = PC->GetPlayerState<AZPlayerState>())
	{
		return PS->GetSkillsComponent()->HasSkill(RequiredSkill);
	}
	return false;
}

FString FZEventCondition_Skill::ToString() const
{
	return FString::Printf(TEXT("Has skill %s"), *ENUM_TO_STRING(EZSkill, RequiredSkill));
}

void FZEventReaction_ReceiveItem::Execute(const FZEventRow* EventRow, AZPlayerController* PC, AZNPCAIController* NPCAIController) const
{
	if (ensure(ItemID.IsValid()))
	{
		const AZFPCharacter* Player = PC->GetFPCharacter();
		const AZNPCharacter* NPC = NPCAIController->GetNPCharacter();
		const TArray<FZItemStack> Items = {FZItemStack(ItemID, Count)};
		FZTradeRequest TradeRequest;
		TradeRequest.NPCItems = Items;
		TradeRequest.NPCInventory = NPC->GetInventoryComponent();
		Player->GetInventoryComponent()->AcceptTrade(TradeRequest, true, false);
	}
}

FString FZEventReaction_ReceiveItem::ToString() const
{
	const FString ItemString = ItemID.IsValid() ? ItemID.ToString() : TEXT("Invalid");
	if (Count != 1)
	{
		return FString::Printf(TEXT("Receive %d of %s"), Count, *ItemString);
	}
	else
	{
		return FString::Printf(TEXT("Receive %s"), *ItemString);
	}
}

void FZEventReaction_GiveItem::Execute(const FZEventRow* EventRow, AZPlayerController* PC, AZNPCAIController* NPCAIController) const
{
	if (ensure(ItemID.IsValid()))
	{
		const AZFPCharacter* Player = PC->GetFPCharacter();
		const AZNPCharacter* NPC = NPCAIController->GetNPCharacter();
		const TArray<FZItemStack> Items = {FZItemStack(ItemID, Count)};
		FZTradeRequest TradeRequest;
		TradeRequest.PlayerItems = Items;
		TradeRequest.NPCInventory = NPC->GetInventoryComponent();
		ensure(Player->GetInventoryComponent()->AcceptTrade(TradeRequest, true, true));
	}
}

FString FZEventReaction_GiveItem::ToString() const
{
	const FString ItemString = ItemID.IsValid() ? ItemID.ToString() : TEXT("Invalid");
	if (Count != 1)
	{
		return FString::Printf(TEXT("Give %d of %s"), Count, *ItemString);
	}
	else
	{
		return FString::Printf(TEXT("Give %s"), *ItemString);
	}
}

void FZEventReaction_GainXP::Execute(const FZEventRow* EventRow, AZPlayerController* PC, AZNPCAIController* NPCAIController) const
{
	if (AZPlayerState* PS = PC->GetPlayerState<AZPlayerState>())
	{
		PS->GetLevelingComponent()->AddXP(XP);
	}
}

FString FZEventReaction_GainXP::ToString() const
{
	return FString::Printf(TEXT("Gain %d XP"), XP);
}

void FZEventReaction_Reputation::Execute(const FZEventRow* EventRow, AZPlayerController* PC, AZNPCAIController* NPCAIController) const
{
	AZGameState* GS = PC->GetWorld()->GetGameState<AZGameState>();
	GS->ChangeReputation(Faction, Change);
}

FString FZEventReaction_Reputation::ToString() const
{
	if (Change >= 0)
	{
		return FString::Printf(TEXT("Gain %f reputation with %s"), Change, *ENUM_TO_STRING(EZFaction, Faction));
	}
	else
	{
		return FString::Printf(TEXT("Lose %f reputation with %s"), Change, *ENUM_TO_STRING(EZFaction, Faction));
	}
}

void FZEventReaction_HomeTransform::Execute(const FZEventRow* EventRow, AZPlayerController* PC, AZNPCAIController* NPCAIController) const
{
	NPCAIController->SetHomeTransform(WaypointTag);
}

FString FZEventReaction_HomeTransform::ToString() const
{
	return FString::Printf(TEXT("Set new home transform to %s"), *WaypointTag.ToString());
}

void FZEventReaction_StartPath::Execute(const FZEventRow* EventRow, AZPlayerController* PC, AZNPCAIController* NPCAIController) const
{
	NPCAIController->StartPath(PathTag, PC, bEscortPlayer);
}

FString FZEventReaction_StartPath::ToString() const
{
	if (bEscortPlayer)
	{
		return FString::Printf(TEXT("Start path %s and escort player"), *PathTag.ToString());
	}
	else
	{
		return FString::Printf(TEXT("Start path %s"), *PathTag.ToString());
	}
}

void FZEventReaction_CancelPath::Execute(const FZEventRow* EventRow, AZPlayerController* PC, AZNPCAIController* NPCAIController) const
{
	NPCAIController->CancelPath(PathTag);
}

FString FZEventReaction_CancelPath::ToString() const
{
	return FString::Printf(TEXT("Cancel path %s"), *PathTag.ToString());
}

void FZEventReaction_Attack::Execute(const FZEventRow* EventRow, AZPlayerController* PC, AZNPCAIController* NPCAIController) const
{
	NPCAIController->Attack(PC);
}

FString FZEventReaction_Attack::ToString() const
{
	return TEXT("Attack player");
}

void FZEventReaction_TakeStolenItems::Execute(const FZEventRow* EventRow, AZPlayerController* PC, AZNPCAIController* NPCAIController) const
{
	NPCAIController->TakeBackStolenItems(PC);
}

FString FZEventReaction_TakeStolenItems::ToString() const
{
	return TEXT("Take back stolen items");
}

void FZEventReaction_WarnAboutBreakingIn::Execute(const FZEventRow* EventRow, AZPlayerController* PC, AZNPCAIController* NPCAIController) const
{
	NPCAIController->WarnAboutBreakingIn(PC);
}

FString FZEventReaction_WarnAboutBreakingIn::ToString() const
{
	return TEXT("Warn about breaking in.");
}



