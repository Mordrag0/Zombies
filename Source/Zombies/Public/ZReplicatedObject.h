// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ZReplicatedObject.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIES_API UZReplicatedObject : public UObject
{
	GENERATED_BODY()
	
public:
	virtual UWorld* GetWorld() const override;

	UFUNCTION(BlueprintPure)
	AActor* GetOwningActor() const { return GetTypedOuter<AActor>(); }
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
	virtual bool CallRemoteFunction(UFunction* Function, void* Parms, struct FOutParmRec* OutParms, FFrame* Stack) override;
	ENetRole GetOwnerRole() const { return GetOwningActor()->GetLocalRole(); }
	void Destroy();

public:
	UFUNCTION()
	bool IsOwnedByLocalRemote() const { return GetOwnerRole() == ROLE_AutonomousProxy; }

	UFUNCTION()
	bool IsSimulatedProxy() const { return GetOwnerRole() == ROLE_SimulatedProxy; }

	UFUNCTION()
	bool IsLocallyOwned() const;

	UFUNCTION()
	bool IsAuthority() const { return GetOwnerRole() == ROLE_Authority; }

	UFUNCTION()
	bool IsNonOwningAuthority() const;

	UFUNCTION(BlueprintPure)
	bool GetReplicates() const { return bReplicates; }
	
protected:
	virtual void OnDestroyed() {}

	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetReplicates)
	bool bReplicates = true;
};

