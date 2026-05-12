// Copyright 2026 Luka Markuš. All rights reserved.


#include "UI/ZActivatableWidget.h"
#include "Input/CommonUIInputTypes.h"
#include "Input/UIActionBinding.h"
#include "UI/ZActivatableWidgetStack.h"


void UZActivatableWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	if (InputAction)
	{
		if (const ULocalPlayer* LP = GetOwningLocalPlayer())
		{
			ActionBindingHandle = FUIActionBinding::TryCreate(*this, FBindUIActionArgs(InputAction, FSimpleDelegate::CreateUObject(this, &ThisClass::RemoveFromStack)), LP->GetIndexInGameInstance());
			if (ActionBindingHandle.IsValid())
			{
				AddActionBinding(ActionBindingHandle);
			}
		}
	}
}

void UZActivatableWidget::NativeOnDeactivated()
{
	if (ActionBindingHandle.IsValid())
	{
		RemoveActionBinding(ActionBindingHandle);
		ActionBindingHandle.Unregister();
	}
	Super::NativeOnDeactivated();
}

bool UZActivatableWidget::NativeOnHandleBackAction()
{
	RemoveFromStack();
	return true;
}

void UZActivatableWidget::SetOwningStack(UZActivatableWidgetStack* InStack)
{
	WeakStack = InStack;
}

void UZActivatableWidget::RemoveFromStack()
{
	if (WeakStack.IsValid())
	{
		WeakStack->RemoveWidget(*this);
	}
	OnRemoved.Broadcast();
}

