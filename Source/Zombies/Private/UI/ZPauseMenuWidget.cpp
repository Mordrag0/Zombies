// Copyright 2026 Luka Markuš. All rights reserved.


#include "UI/ZPauseMenuWidget.h"
#include "ZPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "SaveLoad/ZSaveSubsystem.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UI/Options/ZOptionsWidget.h"
#include "UI/ZButton.h"
#include "UI/ZHUD.h"

void UZPauseMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ResumeButton->OnClicked().AddUObject(this, &ThisClass::Resume);
	RestartButton->OnClicked().AddUObject(this, &ThisClass::Restart);
	SaveButton->OnClicked().AddUObject(this, &ThisClass::Save);
	LoadButton->OnClicked().AddUObject(this, &ThisClass::Load);
	OptionsButton->OnClicked().AddUObject(this, &ThisClass::ShowOptions);
	QuitButton->OnClicked().AddUObject(this, &ThisClass::Quit);
	
#if WITH_EDITOR
	RestartButton->SetVisibility(ESlateVisibility::Visible);
#else
	RestartButton->SetVisibility(ESlateVisibility::Collapsed);
#endif
}

void UZPauseMenuWidget::Resume()
{
	RemoveFromStack();
}

void UZPauseMenuWidget::Restart()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()));
}

void UZPauseMenuWidget::Save()
{
	if (UZSaveSubsystem* SaveSystem = GetWorld()->GetGameInstance()->GetSubsystem<UZSaveSubsystem>())
	{
		SaveSystem->SaveGame(TEXT("SaveGame"));
	}
	RemoveFromStack();
}

void UZPauseMenuWidget::Load()
{
	if (UZSaveSubsystem* SaveSystem = GetWorld()->GetGameInstance()->GetSubsystem<UZSaveSubsystem>())
	{
		SaveSystem->LoadGame(TEXT("SaveGame"));
	}
	RemoveFromStack();
}

void UZPauseMenuWidget::ShowOptions()
{
	OptionsWidget = GetOwningPlayer<AZPlayerController>()->GetHUD<AZHUD>()->PushActivatableWidget<UZOptionsWidget>(EZActivatableWidgetType::Menu, OptionsWidgetClass);
	if (OptionsWidget)
	{
		BIND_MULTICAST(OnOptionsWidgetDeactivatedConnection, OptionsWidget, OptionsWidget->OnRemoved,
			[WeakThis = MakeWeakObjectPtr(this)]()
		{
			if (WeakThis.IsValid())
			{
				WeakThis->OptionsWidget = nullptr;
				WeakThis->OnOptionsWidgetDeactivatedConnection.Disconnect();
			}
		});
	}
}

void UZPauseMenuWidget::Quit()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, false);
}

