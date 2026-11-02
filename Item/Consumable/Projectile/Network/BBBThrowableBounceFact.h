#pragma once
#include "CoreMinimal.h"
#include "BBBThrowableBounceFact.generated.h"

/** 房主确认的最近一次可听碰撞 不保存历史 */
USTRUCT()
struct FBBBThrowableBounceFact final
{
    GENERATED_BODY()
    /** 单个投掷物内递增的碰撞序号 */
    UPROPERTY()
    uint32 Serial = 0;
    /** 实际碰撞位置 */
    UPROPERTY()
    FVector Position = FVector::ZeroVector;
    /** 冲量除以质量得到的速度变化 单位厘米每秒 */
    UPROPERTY()
    float Strength = 0.0f;
};
