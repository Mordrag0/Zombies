// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ZPlayerState.generated.h"

class UZLevelingComponent;
class UZSkillsComponent;
enum class EZSkill : uint8;

/**
 * 
 */
UCLASS()
class ZOMBIES_API AZPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	AZPlayerState();

	UZLevelingComponent* GetLevelingComponent() const { return LevelingComponent; }

	UZSkillsComponent* GetSkillsComponent() const { return SkillsComponent; }

	bool Learn(EZSkill Skill);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UZLevelingComponent> LevelingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UZSkillsComponent> SkillsComponent;
};

