// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "GameplayTagContainer.h"
#include "ZBlackboardKeyType_GameplayTag.generated.h"

class UBlackboardComponent;

/**
 * 
 */
UCLASS(EditInlineNew, meta = (DisplayName = "Gameplay Tag"))
class ZOMBIES_API UZBlackboardKeyType_GameplayTag : public UBlackboardKeyType
{
	GENERATED_BODY()
	
public:
	UZBlackboardKeyType_GameplayTag();

	typedef FGameplayTag FDataType;
	static const FDataType InvalidValue;

	static FGameplayTag GetValue(const UZBlackboardKeyType_GameplayTag* KeyOb, const uint8* RawData);
	static bool SetValue(UZBlackboardKeyType_GameplayTag* KeyOb, uint8* RawData, const FGameplayTag& Value);

	virtual EBlackboardCompare::Type CompareValues(const UBlackboardComponent& OwnerComp, const uint8* MemoryBlock,
		const UBlackboardKeyType* OtherKeyOb, const uint8* OtherMemoryBlock) const override;

	UPROPERTY(EditDefaultsOnly, Category = "Blackboard")
	FGameplayTag DefaultValue = InvalidValue;

protected:
	virtual void InitializeMemory(UBlackboardComponent& OwnerComp, uint8* MemoryBlock) override;
	virtual void Clear(UBlackboardComponent& OwnerComp, uint8* MemoryBlock) override;
	virtual bool IsEmpty(const UBlackboardComponent& OwnerComp, const uint8* MemoryBlock) const override;
	virtual FString DescribeValue(const UBlackboardComponent& OwnerComp, const uint8* RawData) const override;
	virtual bool TestBasicOperation(const UBlackboardComponent& OwnerComp, const uint8* MemoryBlock, EBasicKeyOperation::Type Op) const override;
};

