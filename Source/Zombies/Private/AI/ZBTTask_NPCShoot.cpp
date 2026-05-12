// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZBTTask_NPCShoot.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/ZCharacter.h"
#include "AI/ZNPCAIController.h"
#include "Characters/ZNPCharacter.h"
#include "Weapons/ZRangedWeapon.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Components/ZEquipmentComponent.h"
#include "ZTypes.h"

UZBTTask_NPCShoot::UZBTTask_NPCShoot()
{
	NodeName = "NPC Shoot";

	INIT_TASK_NODE_NOTIFY_FLAGS();
}

EBTNodeResult::Type UZBTTask_NPCShoot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    FZNPCShootMemory* Memory = CastInstanceNodeMemory<FZNPCShootMemory>(NodeMemory);
	
    AZCharacter* Character = GetCharacter(OwnerComp);

	if (!Character)
    {
        return EBTNodeResult::Failed;
    }
    Memory->Weapon = Cast<AZRangedWeapon>(Character->GetEquipmentComponent()->GetEquippedItem());

    if (!Memory->Weapon.IsValid())
    {
        return EBTNodeResult::Failed;
    }

	if (!Memory->Weapon->IsIdle())
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComp = GetBlackboard(OwnerComp);
	AActor* Target = Cast<AActor>(BlackboardComp->GetValueAsObject(GetSelectedBlackboardKey()));
	if (!Target)
	{
		return EBTNodeResult::Failed;
	}


	AZAIController* AIController = GetAIController(OwnerComp);
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}
	AIController->SetFocus(Target);

	Memory->Weapon->FirePressed(EZItemInput::Primary);
	return EBTNodeResult::InProgress;
}

uint16 UZBTTask_NPCShoot::GetInstanceMemorySize() const
{
    return sizeof(FZNPCShootMemory);
}

void UZBTTask_NPCShoot::DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const
{
    const FZNPCShootMemory* Memory = CastInstanceNodeMemory<FZNPCShootMemory>(NodeMemory);

	if (Memory->Weapon.IsValid())
	{
		Values.Add(FString::Printf(TEXT("Weapon: %s (%s)"), *Memory->Weapon->GetName(), *ENUM_TO_STRING(EZItemState, Memory->Weapon->GetItemState())));
	}
}

void UZBTTask_NPCShoot::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
	
	AZAIController* AIController = GetAIController(OwnerComp);
	AIController->ClearFocus(EAIFocusPriority::Gameplay);

    const FZNPCShootMemory* Memory = CastInstanceNodeMemory<FZNPCShootMemory>(NodeMemory);
	Memory->Weapon->FireReleased(EZItemInput::Primary);
}

