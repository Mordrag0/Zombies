// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "ZButton.generated.h"

class UCommonTextBlock;

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API UZButton : public UCommonButtonBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetText(const FText& InText);

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativePreConstruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> ButtonText;

	UPROPERTY(EditAnywhere)
	FText DefaultText;
};

