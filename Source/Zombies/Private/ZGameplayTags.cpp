// Copyright 2026 Luka Markuš. All rights reserved.


#include "ZGameplayTags.h"

namespace ZGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(Event, "Event")
	UE_DEFINE_GAMEPLAY_TAG(NPC, "NPC")
	UE_DEFINE_GAMEPLAY_TAG(Event_Dialogue, "Event.Dialogue")
	UE_DEFINE_GAMEPLAY_TAG(Event_Repeatable, "Event.Repeatable")
	UE_DEFINE_GAMEPLAY_TAG(Waypoint, "Waypoint")
	UE_DEFINE_GAMEPLAY_TAG(Path, "Path")
	UE_DEFINE_GAMEPLAY_TAG(AI_Combat_TargetSet, "AI.Combat.TargetSet")
	UE_DEFINE_GAMEPLAY_TAG(AI_Combat_TargetCleared, "AI.Combat.TargetCleared")
	UE_DEFINE_GAMEPLAY_TAG(AI_Time_HourChanged, "AI.Time.HourChanged")
	UE_DEFINE_GAMEPLAY_TAG(AI_Weapon_OutOfAmmo, "AI.Weapon.OutOfAmmo")
	UE_DEFINE_GAMEPLAY_TAG(AI_Item_EquipComplete, "AI.Item.EquipComplete")
	UE_DEFINE_GAMEPLAY_TAG(AI_Dialogue_TargetSet, "AI.Dialogue.TargetSet")
	UE_DEFINE_GAMEPLAY_TAG(AI_Dialogue_TargetCleared, "AI.Dialogue.TargetCleared")
	UE_DEFINE_GAMEPLAY_TAG(AI_Dialogue_SpottedCriminalSet, "AI.Dialogue.SpottedCriminalSet")
	UE_DEFINE_GAMEPLAY_TAG(AI_Dialogue_SpottedCriminalCleared, "AI.Dialogue.SpottedCriminalCleared")
	UE_DEFINE_GAMEPLAY_TAG(AI_Dialogue_PendingTargetSet, "AI.Dialogue.PendingTargetSet")
	UE_DEFINE_GAMEPLAY_TAG(AI_Dialogue_PendingTargetCleared, "AI.Dialogue.PendingTargetCleared")
	UE_DEFINE_GAMEPLAY_TAG(AI_Path_Started, "AI.Path.Started")
	UE_DEFINE_GAMEPLAY_TAG(AI_Path_Ended, "AI.Path.Ended")
	UE_DEFINE_GAMEPLAY_TAG(AI_Activity_DesiredActivityChanged, "AI.Activity.DesiredActivityChanged")
	UE_DEFINE_GAMEPLAY_TAG(AI_TeleportSucceeded, "AI.TeleportSucceeded")
	UE_DEFINE_GAMEPLAY_TAG(AI_TeleportFailed, "AI.TeleportFailed")
	UE_DEFINE_GAMEPLAY_TAG(AI_HomeTransformChanged, "AI.HomeTransformChanged")
}

