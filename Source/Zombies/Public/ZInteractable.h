// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Misc/TVariant.h"
#include "GameplayTagContainer.h"
#include "ZNetworkTypes.h"
#include "ZInteractable.generated.h"

class IZInteractable;
class AZCharacter;
class AZFPCharacter;
class UZInventoryComponent;
enum class EZCharacterActivity : uint32;
enum class EZDialogueContext : uint8;


DECLARE_MULTICAST_DELEGATE_TwoParams(FZOnInteractionStarted, TScriptInterface<IZInteractable>, TScriptInterface<IZInteractable>); // Always this and target
DECLARE_MULTICAST_DELEGATE_TwoParams(FZOnInteractionStopped, TScriptInterface<IZInteractable>, TScriptInterface<IZInteractable>); // Same as above

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UZInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ZOMBIES_API IZInteractable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.

public:
	virtual bool CanInteract(const AZCharacter* InCharacter) const = 0;

	virtual bool BeginInteract(AZCharacter* InCharacter) = 0;

	virtual void EndInteract(AZCharacter* InCharacter) = 0;

	virtual TArray<UMeshComponent*> GetHighlightMeshes() const = 0;

	virtual FTransform GetPivotPoint() const = 0;

	virtual EZCharacterActivity GetCharacterActivity() const = 0;

	virtual bool ShouldHighlight(const AZFPCharacter* InCharacter) const { return true; }

	virtual FZInteractionParams GetInteractionParams(const AZCharacter* InCharacter) const { return FZInteractionParams(TInPlaceType<FZNoInteractionParams>{}); }

	FZOnInteractionStarted OnInteractionStarted; // Server only
	FZOnInteractionStopped OnInteractionStopped; // Server only
};

