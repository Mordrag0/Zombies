// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ZTypes.generated.h"

#define SURFACE_Default     SurfaceType_Default
#define SURFACE_Body		SurfaceType1
#define SURFACE_Head		SurfaceType2
#define SURFACE_Limb		SurfaceType3

#if !NO_LOGGING
// Only for logging purposes
#define ENUM_TO_STRING(EnumType, EnumValue) \
    (StaticEnum<EnumType>()->GetNameStringByValue(static_cast<int64>(EnumValue)))
#endif

UENUM()
enum class EZZombieAttackType : uint8
{
	LeftHand,
	RightHand,
	BothHands,
};

UENUM()
enum class EZEquipType : uint8
{
	None,
	Melee,
	Pistol,
	Rifle,
	Grenade,
};

UENUM()
enum class EZItemInput : uint8 // Order matters for input priorities
{
	None,
	Primary,
	Secondary,
	Reload,
	ChangeFireMode,
};

UENUM()
enum class EZItemState : uint8
{
	None,
	Down,
	Ready,
	Equipping,
	Unequipping,
	Firing,
	Reloading,
	Holding, // For abilities that are used in 2 parts (hold and release)
};

UENUM()
enum class EZGameplayEvent : uint8
{
	None,
	FinishReload,
	FinishEquip,
	FinishUnequip,
	ReleaseGrenade,
	Equip,
	Unequip,
	PickUp,
};

UENUM()
enum class EZFaction : uint8
{
	NoFaction,
	Player,
	Zombies,
	FactionA,
	FactionB,
	FactionC,
	FactionD,
	MAX UMETA(Hidden)
};

UENUM(Meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EZCharacterActivity : uint32
{
	None         = 0,
	Sleeping     = 1 << 0,
	Sitting      = 1 << 1,
	Looting      = 1 << 2,
	PickingUp    = 1 << 3,
	Lockpicking  = 1 << 4,
	Talking      = 1 << 5,
	Smoking      = 1 << 6,
	Surrendering = 1 << 7,
	Eating       = 1 << 8,
	Drinking     = 1 << 9,
	MAX          = 1 << 10 UMETA(Hidden)
};
ENUM_CLASS_FLAGS(EZCharacterActivity)

UENUM(BlueprintType)
enum class EZCharacterActivityBP : uint8
{
	None,
	Sleeping,
	Sitting,
	Looting,
	PickingUp,
	Lockpicking,
	Talking,
	Smoking,
	Surrendering,
	Eating,
	Drinking,
	MAX UMETA(Hidden)
};


inline bool IsInteractionActivity(EZCharacterActivity Activity)
{
	switch (Activity)
	{
		case EZCharacterActivity::Sleeping:
		case EZCharacterActivity::Sitting:
		case EZCharacterActivity::Looting:
		case EZCharacterActivity::PickingUp:
		case EZCharacterActivity::Lockpicking:
		case EZCharacterActivity::Talking:
			return true;
		case EZCharacterActivity::Smoking:
		case EZCharacterActivity::Surrendering:
		case EZCharacterActivity::Eating:
		case EZCharacterActivity::Drinking:
		case EZCharacterActivity::None:
			return false;
		case EZCharacterActivity::MAX:
		default:
			ensure(0);
			return false;
	}
}

inline EZCharacterActivity ToCharacterActivity(EZCharacterActivityBP Activity)
{
	if (Activity == EZCharacterActivityBP::None)
	{
		return EZCharacterActivity::None;
	}
	return static_cast<EZCharacterActivity>(1 << (static_cast<uint8>(Activity) - 1)); // -1 to account for None offset
}

inline EZCharacterActivityBP ToCharacterActivityBP(EZCharacterActivity Activity)
{
	if (Activity == EZCharacterActivity::None)
	{
		return EZCharacterActivityBP::None;
	}
    for (uint8 Idx = 0; Idx < static_cast<uint8>(EZCharacterActivityBP::MAX); Idx++)
    {
        if (Activity == static_cast<EZCharacterActivity>(1 << Idx))
        {
			return static_cast<EZCharacterActivityBP>(Idx + 1); // +1 to account for None offset
        }
    }
	ensure(0);
	return EZCharacterActivityBP::MAX;
}

static FString ActivityToString(EZCharacterActivity Activities)
{
	FString Result;
	for (uint32 Bit = 1; Bit < static_cast<uint32>(EZCharacterActivity::MAX); Bit <<= 1)
	{
		const EZCharacterActivity Activity = static_cast<EZCharacterActivity>(Bit);
		if (EnumHasAnyFlags(Activities, Activity))
		{
			if (!Result.IsEmpty())
			{
				Result += TEXT(" | ");
			}
			Result += ENUM_TO_STRING(EZCharacterActivity, Activity);
		}
	}
	return Result.IsEmpty() ? TEXT("None") : Result;
}

//UENUM(BlueprintType)
//enum class EZCharacterActivityContext : uint8
//{
//	None,
//	Bed, 
//	Chair,
//	Floor,
//};

UENUM()
enum class EZSkill : uint8
{
	Lockpicking,
	Hacking,
};

UENUM()
enum class EZDialogueContext : uint8
{
	None,
	Confrontation_Stealing,
	Confrontation_BreakingIn,
	MAX UMETA(Hidden)
};

UENUM()
enum class EZNotificationType : uint8
{
	Info,
	XPEarned,
	LeveledUp,
	SkillUnlocked,
	ItemGiven,
	ItemReceived,
	ReputationChange,
};

UENUM(BlueprintType)
enum class EZMovementMode : uint8
{
	Walking,
	Falling,
	Flying,
	Swimming,
	Sliding,
};
