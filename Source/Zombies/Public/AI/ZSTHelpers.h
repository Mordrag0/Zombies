// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "StateTreePropertyBindings.h"

#if WITH_EDITOR

#define GET_PROPERTY_DISPLAY_TEXT(PropertyName, FallbackExpr) \
    ([&]() -> FText { \
        FText Result = BindingLookup.GetBindingSourceDisplayName( \
            FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, PropertyName)), Formatting); \
        if (Result.IsEmpty()) \
        { \
            const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>(); \
            check (InstanceData); \
            if (InstanceData) Result = (FallbackExpr); \
        } \
        return Result; \
    }())

#define GET_OBJECT_DISPLAY_TEXT(PropertyName) \
    GET_PROPERTY_DISPLAY_TEXT(PropertyName, \
        InstanceData->PropertyName \
            ? FText::FromString(InstanceData->PropertyName->GetName()) \
            : FText::GetEmpty())

#define GET_WEAK_OBJECT_DISPLAY_TEXT(PropertyName) \
    GET_PROPERTY_DISPLAY_TEXT(PropertyName, \
        InstanceData->PropertyName.IsValid() \
            ? FText::FromString(InstanceData->PropertyName->GetName()) \
            : FText::GetEmpty())

#define GET_INTERFACE_DISPLAY_TEXT(PropertyName) \
    GET_PROPERTY_DISPLAY_TEXT(PropertyName, \
        InstanceData->PropertyName \
            ? FText::FromString(InstanceData->PropertyName.GetObject()->GetName()) \
            : FText::GetEmpty())

#define GET_VECTOR_DISPLAY_TEXT(PropertyName) \
    GET_PROPERTY_DISPLAY_TEXT(PropertyName, \
        FAISystem::IsValidLocation(InstanceData->PropertyName) \
            ? FText::FromString(InstanceData->PropertyName.ToString()) \
            : FText::GetEmpty())

#define GET_ORIENTATION_DISPLAY_TEXT(PropertyName) \
    GET_PROPERTY_DISPLAY_TEXT(PropertyName, \
        FAISystem::IsValidOrientation(InstanceData->PropertyName) \
            ? FText::FromString(InstanceData->PropertyName.ToString()) \
            : FText::GetEmpty())

#define GET_ROTATION_DISPLAY_TEXT(PropertyName) \
    GET_PROPERTY_DISPLAY_TEXT(PropertyName, \
        FAISystem::IsValidRotation(InstanceData->PropertyName) \
            ? FText::FromString(InstanceData->PropertyName.ToString()) \
            : FText::GetEmpty())

#define GET_ENUM_DISPLAY_TEXT(EnumType, PropertyName) \
    GET_PROPERTY_DISPLAY_TEXT(PropertyName, \
        FText::FromString(ENUM_TO_STRING(EnumType, InstanceData->PropertyName)))

#define GET_NUMBER_DISPLAY_TEXT(PropertyName) \
    GET_PROPERTY_DISPLAY_TEXT(PropertyName, \
        FText::AsNumber(InstanceData->PropertyName))

#define GET_STRING_DISPLAY_TEXT(PropertyName) \
    GET_PROPERTY_DISPLAY_TEXT(PropertyName, \
        FText::FromString(InstanceData->PropertyName))

#define GET_GAMEPLAYTAG_DISPLAY_TEXT(PropertyName) \
    GET_PROPERTY_DISPLAY_TEXT(PropertyName, \
        InstanceData->PropertyName.IsValid() \
            ? FText::FromString(InstanceData->PropertyName.ToString()) \
            : FText::GetEmpty())

FString StripRichTextTags(const FString& RichText);
FText JoinParameters(const TArray<FText>& Params);
#endif

#if WITH_EDITOR
#define STATETREE_NODE_DESCRIPTION() \
virtual FString GetDescriptionLabel() const override \
    { \
        FString Label = StaticStruct()->GetMetaData(TEXT("DisplayName")); \
        Label.RemoveFromEnd(TEXT("?")); \
        return TEXT("<b>") + Label + TEXT("</>"); \
    }
#else
#define STATETREE_NODE_DESCRIPTION()
#endif
