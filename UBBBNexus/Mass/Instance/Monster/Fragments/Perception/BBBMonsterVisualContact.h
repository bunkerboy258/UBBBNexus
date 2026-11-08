#pragma once

#include "CoreMinimal.h"
#include "BBBMonsterVisualContact.generated.h"

/** 对一个玩家的当前视觉警觉与最近确认结果 不保存观察历史 */
USTRUCT()
struct FBBBMonsterVisualContact final
{
    GENERATED_BODY()

    /** 玩家对象仅用于有效性和身份检查 */
    TWeakObjectPtr<AActor> Actor;

    /** 未确认目标的当前警觉比例 */
    float Awareness = 0.0f;

    /** 最近一次视觉确认位置 */
    FVector Position = FVector::ZeroVector;

    /** 最近一次视觉确认时间 */
    float ConfirmedAt = -FLT_MAX;

    /** 当前已完成视觉确认 */
    bool bConfirmed = false;

    /** 本轮有无遮挡的前方视觉 */
    bool bVisible = false;

    /** 暂时排除当前位置的结束时间 */
    float ExcludedUntil = 0.0f;

    /** 被判定不可达的位置 */
    FVector ExcludedPosition = FVector::ZeroVector;
};
