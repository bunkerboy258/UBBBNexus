#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Network/BBBThrowableDamageFact.h"
#include "BBBThrowableExplosionFact.generated.h"

/** 单个投掷物唯一的终态结果 不保存事件历史 */
USTRUCT()
struct FBBBThrowableExplosionFact final
{
    GENERATED_BODY()
    UPROPERTY()
    bool bExploded = false;
    UPROPERTY()
    FVector Position = FVector::ZeroVector;
    UPROPERTY()
    int32 PlayerId = INDEX_NONE;
    UPROPERTY()
    double Time = 0.0;
    UPROPERTY()
    TArray<FBBBThrowableDamageFact> Targets;
};
