// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZBTDecorator_BlackboardBase.h"
#include "AISystem.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

UZBTDecorator_BlackboardBase::UZBTDecorator_BlackboardBase()
{
	NodeName = "ZBTBlackboardBase";

	INIT_DECORATOR_NODE_NOTIFY_FLAGS();

	BlackboardKey.AllowNoneAsValue(GET_AI_CONFIG_VAR(bBlackboardKeyDecoratorAllowsNoneAsValue));
}

void UZBTDecorator_BlackboardBase::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (const UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		BlackboardKey.ResolveSelectedKey(*BBAsset);
	}
	else
	{
		BlackboardKey.InvalidateResolvedKey();
	}
}

EBlackboardNotificationResult UZBTDecorator_BlackboardBase::OnBlackboardKeyValueChange(const UBlackboardComponent& Blackboard, FBlackboard::FKey ChangedKeyID)
{
	UBehaviorTreeComponent* BehaviorComp = static_cast<UBehaviorTreeComponent*>(Blackboard.GetBrainComponent());
	if (BehaviorComp == nullptr)
	{
		return EBlackboardNotificationResult::RemoveObserver;
	}

	if (BlackboardKey.GetSelectedKeyID() == ChangedKeyID)
	{
		// Only request execution if condition actually changed
		const bool bConditionMet = CalculateRawConditionValue(*BehaviorComp, nullptr); // Base class passes null - subclasses using node memory should override this
		if (bConditionMet == IsInversed())
		{
			BehaviorComp->RequestExecution(this);
		}
	}
	return EBlackboardNotificationResult::ContinueObserving;
}

#if WITH_EDITOR
FName UZBTDecorator_BlackboardBase::GetNodeIconName() const
{
	return FName("BTEditor.Graph.BTNode.Decorator.Blackboard.Icon");
}

FString UZBTDecorator_BlackboardBase::GetErrorMessage() const
{
	if (GetBlackboardAsset() == nullptr)
	{
		return UE::BehaviorTree::Messages::BlackboardNotSet.ToString();
	}
	return Super::GetErrorMessage();
}
#endif	// WITH_EDITOR

void UZBTDecorator_BlackboardBase::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp)
	{
		auto KeyID = BlackboardKey.GetSelectedKeyID();
		BlackboardComp->RegisterObserver(KeyID, this, FOnBlackboardChangeNotification::CreateUObject(this, &UZBTDecorator_BlackboardBase::OnBlackboardKeyValueChange));
	}
}

void UZBTDecorator_BlackboardBase::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp)
	{
		BlackboardComp->UnregisterObserversFrom(this);
	}
}

