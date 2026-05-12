// Copyright 2026 Luka Markuš. All rights reserved.


#include "Events/ZQuest.h"
#include "Algo/AnyOf.h"

bool FZQuestRow::RequiresObjective(FGameplayTag Event) const
{
    return Algo::AnyOf(Objectives, [&Event](const FZQuestObjective& Objective)
    {
        return Objective.RequiredEvent == Event;
    });
}

bool FZQuestRow::AreAllObjectivesCompleted(const FGameplayTagContainer& CompletedEvents) const
{
	for (const FZQuestObjective& Objective : Objectives)
	{
		if (!CompletedEvents.HasTagExact(Objective.RequiredEvent))
		{
			return false;
		}
	}
	return true;
}

