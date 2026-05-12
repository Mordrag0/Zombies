// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UI/ZActivatableWidgetStack.h"
#include "ZRootLayoutWidget.h"
#include "ZHUD.generated.h"

class UZHUDWidget;
class AZFPCharacter;
enum class EZNotificationType : uint8;

/**
 * 
 */
UCLASS(Abstract)
class ZOMBIES_API AZHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	template<typename T> 
	T* PushActivatableWidget(EZActivatableWidgetType WidgetType, TSubclassOf<T> WidgetClass);

	UZHUDWidget* GetHUDWidget() const { return HUDWidget; }
	
	void AddNotification(EZNotificationType NotificationType, const FText& Message);

	void Init(AZFPCharacter* FPCharacter);
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	TObjectPtr<UZRootLayoutWidget> RootLayout;

	UPROPERTY()
	TObjectPtr<UZHUDWidget> HUDWidget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UZRootLayoutWidget> RootLayoutClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UZHUDWidget> HUDWidgetClass;
};


template<typename T>
T* AZHUD::PushActivatableWidget(EZActivatableWidgetType WidgetType, TSubclassOf<T> WidgetClass)
{
	if (!RootLayout)
	{
		return nullptr;
	}
	UZActivatableWidgetStack* Stack = RootLayout->GetWidgetStack(WidgetType);
	check(Stack);
	ensure(WidgetClass && WidgetClass->IsChildOf(T::StaticClass()));
	return Stack->AddWidget<T>(WidgetClass);
}
