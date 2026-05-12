// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZBTDecorator_ShouldTalkTo.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

UZBTDecorator_ShouldTalkTo::UZBTDecorator_ShouldTalkTo()
{
	NodeName = "ShouldTalkTo";
	
    INIT_DECORATOR_NODE_NOTIFY_FLAGS();
	
	PendingDialogueTargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, PendingDialogueTargetKey), AActor::StaticClass());
	LookAtKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, LookAtKey), AActor::StaticClass());
	LocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, LocationKey));
	Distance = 500.f;

	bTickIntervals = true;
}

void UZBTDecorator_ShouldTalkTo::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (const UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		PendingDialogueTargetKey.ResolveSelectedKey(*BBAsset);
		LookAtKey.ResolveSelectedKey(*BBAsset);
		LocationKey.ResolveSelectedKey(*BBAsset);
	}
	else
	{
		PendingDialogueTargetKey.InvalidateResolvedKey();
		LookAtKey.InvalidateResolvedKey();
		LocationKey.InvalidateResolvedKey();
	}
}

bool UZBTDecorator_ShouldTalkTo::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	return (ShouldTalkTo(OwnerComp) == EZShouldTalkToState::Close);
}

void UZBTDecorator_ShouldTalkTo::DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const
{
	Super::DescribeRuntimeValues(OwnerComp, NodeMemory, Verbosity, Values);

	const UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	if (BlackboardComp)
	{
		const FString PendingDialogueTargetKeyValue = BlackboardComp->DescribeKeyValue(PendingDialogueTargetKey.GetSelectedKeyID(), EBlackboardDescription::OnlyValue);
		Values.Add(FString::Printf(TEXT("PendingDialogueTarget: %s"), *PendingDialogueTargetKeyValue));
		const FString LookAtKeyValue = BlackboardComp->DescribeKeyValue(LookAtKey.GetSelectedKeyID(), EBlackboardDescription::OnlyValue);
		Values.Add(FString::Printf(TEXT("Look at: %s"), *LookAtKeyValue));
		const FString LocationKeyValue = BlackboardComp->DescribeKeyValue(LocationKey.GetSelectedKeyID(), EBlackboardDescription::OnlyValue);
		Values.Add(FString::Printf(TEXT("Location: %s"), *LocationKeyValue));
	}
}

FString UZBTDecorator_ShouldTalkTo::GetStaticDescription() const
{
	FString PendingDialogueKeyDesc("invalid");
	if (PendingDialogueTargetKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		PendingDialogueKeyDesc = PendingDialogueTargetKey.SelectedKeyName.ToString();
	}
	FString LookAtKeyDesc("invalid");
	if (LookAtKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		LookAtKeyDesc = LookAtKey.SelectedKeyName.ToString();
	}
	FString LocationKeyDesc("invalid");
	if (LocationKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		LocationKeyDesc = LocationKey.SelectedKeyName.ToString();
	}

	return FString::Printf(TEXT("%s: %s %s %s"), *Super::GetStaticDescription(), *PendingDialogueKeyDesc, *LookAtKeyDesc, *LocationKeyDesc);
}

EBlackboardNotificationResult UZBTDecorator_ShouldTalkTo::OnBlackboardKeyValueChange(const UBlackboardComponent& Blackboard, FBlackboard::FKey ChangedKeyID)
{
	UBehaviorTreeComponent* BehaviorComp = static_cast<UBehaviorTreeComponent*>(Blackboard.GetBrainComponent());
	if (!BehaviorComp)
	{
		return EBlackboardNotificationResult::RemoveObserver;
	}

	if ((PendingDialogueTargetKey.GetSelectedKeyID() == ChangedKeyID) || (LookAtKey.GetSelectedKeyID() == ChangedKeyID) || (LocationKey.GetSelectedKeyID() == ChangedKeyID))
	{
		BehaviorComp->RequestExecution(this);
	}
	return EBlackboardNotificationResult::ContinueObserving;
}

#if WITH_EDITOR
FString UZBTDecorator_ShouldTalkTo::GetErrorMessage() const
{
	if (!GetBlackboardAsset())
	{
		return UE::BehaviorTree::Messages::BlackboardNotSet.ToString();
	}
	return Super::GetErrorMessage();
}
#endif

uint16 UZBTDecorator_ShouldTalkTo::GetInstanceMemorySize() const
{
	return sizeof(FZShouldTalkToMemory);
}

void UZBTDecorator_ShouldTalkTo::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FZShouldTalkToMemory* Memory = CastInstanceNodeMemory<FZShouldTalkToMemory>(NodeMemory);
	const EZShouldTalkToState Result = ShouldTalkTo(OwnerComp);
	Memory->bWasClose = Result == EZShouldTalkToState::Close;

	if (UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent())
	{
		FBlackboard::FKey PendingDialogueKeyID = PendingDialogueTargetKey.GetSelectedKeyID();
		BlackboardComp->RegisterObserver(PendingDialogueKeyID, this, FOnBlackboardChangeNotification::CreateUObject(this, &ThisClass::OnBlackboardKeyValueChange));
		FBlackboard::FKey LookAtKeyID = LookAtKey.GetSelectedKeyID();
		BlackboardComp->RegisterObserver(LookAtKeyID, this, FOnBlackboardChangeNotification::CreateUObject(this, &ThisClass::OnBlackboardKeyValueChange));
		FBlackboard::FKey LocationKeyID = LocationKey.GetSelectedKeyID();
		BlackboardComp->RegisterObserver(LocationKeyID, this, FOnBlackboardChangeNotification::CreateUObject(this, &ThisClass::OnBlackboardKeyValueChange));
	}

	if (Result == EZShouldTalkToState::NoTarget)
	{
		SetNextTickTime(NodeMemory, FLT_MAX);
	}
	else
	{
		SetNextTickTime(NodeMemory, .2f);
	}
}

void UZBTDecorator_ShouldTalkTo::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent())
	{
		BlackboardComp->UnregisterObserversFrom(this);
	}
}

void UZBTDecorator_ShouldTalkTo::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	//Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	FZShouldTalkToMemory* Memory = CastInstanceNodeMemory<FZShouldTalkToMemory>(NodeMemory);

	const EZShouldTalkToState Result = ShouldTalkTo(OwnerComp);
	const bool bClose = Result == EZShouldTalkToState::Close;
	if (bClose != Memory->bWasClose)
	{
		Memory->bWasClose = bClose;
		OwnerComp.RequestExecution(this);
	}
	if (Result == EZShouldTalkToState::NoTarget)
	{
		SetNextTickTime(NodeMemory, FLT_MAX);
	}
	else
	{
		SetNextTickTime(NodeMemory, .2f);
	}
}

EZShouldTalkToState UZBTDecorator_ShouldTalkTo::ShouldTalkTo(UBehaviorTreeComponent& OwnerComp) const
{
	const UBlackboardComponent* Blackboard = GetBlackboard(OwnerComp);
	const UObject* PendingDialogueTarget = Blackboard->GetValueAsObject(PendingDialogueTargetKey.SelectedKeyName);
	if (!PendingDialogueTarget)
	{
		return EZShouldTalkToState::NoTarget;
	}
	const AActor* LookAtActor = Cast<AActor>(Blackboard->GetValueAsObject(LookAtKey.SelectedKeyName));
	if (!LookAtActor)
	{
		return EZShouldTalkToState::NoTarget;
	}
	const FVector Location = Blackboard->GetValueAsVector(LocationKey.SelectedKeyName);
	return (FVector::DistSquared(LookAtActor->GetActorLocation(), Location) < FMath::Square(Distance)) 
		? EZShouldTalkToState::Close : EZShouldTalkToState::Far;
}

