// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZInteractableBase.h"
#include "ZLockableBase.generated.h"

class AZCharacter;

UCLASS(Abstract)
class ZOMBIES_API AZLockableBase : public AZInteractableBase
{
	GENERATED_BODY()
	
public:	
	AZLockableBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void Lock();

	virtual void Unlock();

	void Open();

	void Close();
	
	UFUNCTION(BlueprintPure)
	bool GetOpen() const { return bOpen; }

	bool GetLocked() const { return bLocked; }

	FTransform GetLockTransform() const { return LockTransformComponent->GetComponentTransform(); }

	virtual bool CanInteract(const AZCharacter* Character) const override;

	virtual FTransform GetPivotPoint() const override;
	virtual EZCharacterActivity GetCharacterActivity() const override;

protected:

private:
	UPROPERTY(EditAnywhere, Replicated)
	bool bLocked;

	UPROPERTY(EditAnywhere, Replicated)
	bool bOpen;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> LockTransformComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> PivotComponent;
};

