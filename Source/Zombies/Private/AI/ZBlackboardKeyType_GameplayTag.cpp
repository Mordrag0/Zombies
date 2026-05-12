// Copyright 2026 Luka Markuš. All rights reserved.


#include "AI/ZBlackboardKeyType_GameplayTag.h"

const UZBlackboardKeyType_GameplayTag::FDataType UZBlackboardKeyType_GameplayTag::InvalidValue = FGameplayTag::EmptyTag;

UZBlackboardKeyType_GameplayTag::UZBlackboardKeyType_GameplayTag()
{
	ValueSize = sizeof(FGameplayTag);
	SupportedOp = EBlackboardKeyOperation::Basic;
}

FGameplayTag UZBlackboardKeyType_GameplayTag::GetValue(const UZBlackboardKeyType_GameplayTag* KeyOb, const uint8* RawData)
{
	return GetValueFromMemory<FGameplayTag>(RawData);
}

bool UZBlackboardKeyType_GameplayTag::SetValue(UZBlackboardKeyType_GameplayTag* KeyOb, uint8* RawData, const FGameplayTag& Value)
{
	return SetValueInMemory<FGameplayTag>(RawData, Value);
}

EBlackboardCompare::Type UZBlackboardKeyType_GameplayTag::CompareValues(const UBlackboardComponent& OwnerComp, const uint8* MemoryBlock, const UBlackboardKeyType* OtherKeyOb, const uint8* OtherMemoryBlock) const
{
	const FGameplayTag MyValue = GetValue(this, MemoryBlock);
	const FGameplayTag OtherValue = GetValue(reinterpret_cast<const UZBlackboardKeyType_GameplayTag*>(OtherKeyOb), OtherMemoryBlock);

	return (MyValue == OtherValue) ? EBlackboardCompare::Equal : EBlackboardCompare::NotEqual;
}

void UZBlackboardKeyType_GameplayTag::InitializeMemory(UBlackboardComponent& OwnerComp, uint8* MemoryBlock)
{
	SetValue(this, MemoryBlock, DefaultValue);
}

void UZBlackboardKeyType_GameplayTag::Clear(UBlackboardComponent& OwnerComp, uint8* MemoryBlock)
{
	SetValue(this, MemoryBlock, FGameplayTag::EmptyTag);
}

bool UZBlackboardKeyType_GameplayTag::IsEmpty(const UBlackboardComponent& OwnerComp, const uint8* MemoryBlock) const
{
	const FGameplayTag Value = GetValue(this, MemoryBlock);
	return !Value.IsValid();
}

FString UZBlackboardKeyType_GameplayTag::DescribeValue(const UBlackboardComponent& OwnerComp, const uint8* RawData) const
{
	const FGameplayTag CurrentValue = GetValue(this, RawData);
	return CurrentValue.ToString();
}

bool UZBlackboardKeyType_GameplayTag::TestBasicOperation(const UBlackboardComponent& OwnerComp, const uint8* MemoryBlock, EBasicKeyOperation::Type Op) const
{
	const FGameplayTag Value = GetValueFromMemory<FGameplayTag>(MemoryBlock);
	return (Op == EBasicKeyOperation::Set) ? Value.IsValid() : !Value.IsValid();
}

