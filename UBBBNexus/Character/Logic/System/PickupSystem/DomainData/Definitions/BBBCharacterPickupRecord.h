#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/PickupSystem/DomainData/Definitions/BBBCharacterPickupReceipt.h"
#include "BBBCharacterPickupRecord.generated.h"

/** 仍然有效来源的最近一次交付处理记录 */
USTRUCT()
struct FBBBCharacterPickupRecord final
{
    GENERATED_BODY()
    /** 最新交付及其最终结果 */
    UPROPERTY()
    FBBBCharacterPickupReceipt Receipt;
    /** 是否等待 ItemSystem 生成结果 */
    UPROPERTY()
    bool bAwaitingResult = false;
    /** 本掉落物入包被拒绝后的下次领取时间 */
    double RetryAt = 0.0;
};
