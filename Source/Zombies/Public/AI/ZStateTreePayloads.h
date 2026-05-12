// Copyright 2026 Luka Markuš. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZStateTreePayloads.generated.h"


USTRUCT()
struct FZHourChangedPayload
{
    GENERATED_BODY()

	FZHourChangedPayload() {}
	FZHourChangedPayload(int32 InHour) : Hour(InHour) {}
    
    UPROPERTY(VisibleAnywhere)
    int32 Hour = 0;
};

