// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ZEditorUtils.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIESEDITOR_API UZEditorUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "StateTree")
	static TArray<FString> FindStatesWithNode(UStateTree* StateTree, const FString& TaskName);
};

