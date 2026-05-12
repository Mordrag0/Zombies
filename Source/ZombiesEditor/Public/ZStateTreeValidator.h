// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorValidatorBase.h"
#include "ZStateTreeValidator.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIESEDITOR_API UZStateTreeValidator : public UEditorValidatorBase
{
	GENERATED_BODY()
	
public:
	virtual bool CanValidateAsset_Implementation(const FAssetData& InAssetData, UObject* InObject, FDataValidationContext& InContext) const;
	
	/** Override this to validate in C++ with access to FDataValidationContext */
	virtual EDataValidationResult ValidateLoadedAsset_Implementation(const FAssetData& InAssetData, UObject* InAsset, FDataValidationContext& Context);
};

