// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Internationalization/Text.h"

namespace ZText
{
    // Common
    const FText Yes = NSLOCTEXT("Zombies", "Yes", "Yes");
    const FText No = NSLOCTEXT("Zombies", "No", "No");
    const FText Invalid = NSLOCTEXT("Zombies", "Invalid", "Invalid");
    
    // Interaction
    const FText NoLockpicks = NSLOCTEXT("Zombies", "NoLockpicks", "No lockpicks!");
    
    // Notifications
    const FText GainedReputation = NSLOCTEXT("Zombies", "GainedReputation", "Gained {0} reputation with {1}");
    const FText LostReputation = NSLOCTEXT("Zombies", "LostReputation", "Lost {0} reputation with {1}");
    const FText ItemReceived = NSLOCTEXT("Zombies", "ReceivedItem", "Received {0} x{1}");
    const FText ItemGiven = NSLOCTEXT("Zombies", "ReceivedItem", "Given {0} x{1}");
    
    const FText XPEarned = NSLOCTEXT("Zombies", "XPEarned", "+{0} XP");
    const FText SkillUnlocked = NSLOCTEXT("Zombies", "SkillUnlocked", "Learned new skill: {0}");
    
    
    const FText NoFaction = NSLOCTEXT("Zombies", "NoFaction", "NoFaction");
    const FText PlayerFaction = NSLOCTEXT("Zombies", "PlayerFaction", "PlayerFaction");
    const FText ZombiesFaction = NSLOCTEXT("Zombies", "ZombiesFaction", "ZombiesFaction");
    const FText FactionA = NSLOCTEXT("Zombies", "FactionA", "FactionA");
    const FText FactionB = NSLOCTEXT("Zombies", "FactionB", "FactionB");
    const FText FactionC = NSLOCTEXT("Zombies", "FactionC", "FactionC");
    const FText FactionD = NSLOCTEXT("Zombies", "FactionD", "FactionD");
}

