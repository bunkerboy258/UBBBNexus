#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Combat/FBBBHitContribution.h"
#include "BBBCharacterDamageDeliveryState.generated.h"

class APawn;

/** 本帧已发生且需要投送到控制者的命中事实 */
USTRUCT()
struct FBBBCharacterDamageDeliveryState final
{
    GENERATED_BODY()

    /** 本地事实批次编号 */
    uint64 Serial = 0;

    /** 按提交顺序保存每个完整命中 */
    TArray<FBBBHitContribution> Hits;

};
