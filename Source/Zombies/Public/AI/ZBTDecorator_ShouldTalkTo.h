// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AI/ZBTDecorator_Base.h"
#include "ZBTDecorator_ShouldTalkTo.generated.h"

struct FZShouldTalkToMemory //: public FBTAuxiliaryMemory
{
    bool bWasClose;
};

enum class EZShouldTalkToState : uint8
{
	NoTarget,
	Far,
	Close
};

/**
 * 
 */
UCLASS()
class ZOMBIES_API UZBTDecorator_ShouldTalkTo : public UZBTDecorator_Base
{
	GENERATED_BODY()
	
public:
	UZBTDecorator_ShouldTalkTo();

	/** initialize any asset related data */
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual void DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const override;
	virtual FString GetStaticDescription() const override;

#if WITH_EDITOR
	virtual FString GetErrorMessage() const override;
#endif

	virtual uint16 GetInstanceMemorySize() const override;

protected:
	/** notify about change in blackboard keys */
	virtual EBlackboardNotificationResult OnBlackboardKeyValueChange(const UBlackboardComponent& Blackboard, FBlackboard::FKey ChangedKeyID);

	/** called when execution flow controller becomes active */
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	/** called when execution flow controller becomes inactive */
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	EZShouldTalkToState ShouldTalkTo(UBehaviorTreeComponent& OwnerComp) const;

	UPROPERTY(EditAnywhere, Category=Blackboard)
	FBlackboardKeySelector PendingDialogueTargetKey;

	UPROPERTY(EditAnywhere, Category=Blackboard)
	FBlackboardKeySelector LookAtKey;

	UPROPERTY(EditAnywhere, Category=Blackboard)
	FBlackboardKeySelector LocationKey;

	UPROPERTY(EditAnywhere)
	float Distance;
};

