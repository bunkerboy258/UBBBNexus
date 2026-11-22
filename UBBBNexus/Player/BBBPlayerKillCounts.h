#pragma once

#include "CoreMinimal.h"
#include "BBBPlayerKillCounts.generated.h"

/** 本局由玩家确认击杀的两类目标数量 */
USTRUCT(BlueprintType)
struct FBBBPlayerKillCounts
{
    GENERATED_BODY()

    /** 小怪击杀数量 */
    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    int32 CommonKills = 0;

    /** 特感击杀数量 */
    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    int32 SpecialKills = 0;
};
