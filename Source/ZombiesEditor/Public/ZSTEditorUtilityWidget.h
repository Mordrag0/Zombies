// Copyright 2026 Luka Markuš All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "ZSTEditorUtilityWidget.generated.h"

class UEditorUtilityButton;
class UStateTree;

/**
 * 
 */
UCLASS()
class ZOMBIESEDITOR_API UZSTEditorUtilityWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UFUNCTION()
	void Test() const;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditorUtilityButton> TestButton;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStateTree> AsStateTree;
};

