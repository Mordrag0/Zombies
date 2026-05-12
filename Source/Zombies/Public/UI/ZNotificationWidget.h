// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "ZNotificationWidget.generated.h"

class UImage;
class UCommonTextBlock;
struct FZNotificationStyle;

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API UZNotificationWidget : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	void Show(const FText& Message, FZNotificationStyle* Style);

protected:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> Text;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Icon;
};

