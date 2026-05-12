// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "ZActivatableWidget.generated.h"

class UInputAction;
class UZActivatableWidgetStack;

DECLARE_MULTICAST_DELEGATE(FZOnActivatableWidgetRemoved);

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API UZActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	FZOnActivatableWidgetRemoved OnRemoved;

	void SetOwningStack(UZActivatableWidgetStack* InStack);
	
	virtual void RemoveFromStack();

protected:
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
	
	virtual bool NativeOnHandleBackAction() override;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> InputAction;

	FUIActionBindingHandle ActionBindingHandle;

	TWeakObjectPtr<UZActivatableWidgetStack> WeakStack;
};

