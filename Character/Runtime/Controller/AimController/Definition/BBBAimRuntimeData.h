
#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Runtime/Controller/AimController/Definition/States/BBBAimStates.h"
#include "BBBAimRuntimeData.generated.h"

USTRUCT(BlueprintType)
//瞄准状态
struct FBBBAimRuntimeData
{
    GENERATED_BODY()

    UPROPERTY(Transient)
    FBBBAimRuntimeState State;
};
