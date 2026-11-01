#pragma once

#include "CoreMinimal.h"
#include "BBBCharacterPhysicalImpact.generated.h"

/** 一次命中携带的全身物理冲击 普通命中使用零值 */
USTRUCT()
struct FBBBCharacterPhysicalImpact final
{
    GENERATED_BODY()

    /** 造成冲击的小怪本代身份 */
    UPROPERTY()
    FGuid SourceId;

    /** 该来源当前动作编号 */
    UPROPERTY()
    uint32 AttackId = 0;

    /** 需要叠加的世界线速度 厘米每秒 */
    UPROPERTY()
    FVector Velocity = FVector::ZeroVector;

    /** 最短布娃娃保持秒数 零表示没有击倒 */
    UPROPERTY()
    float HoldSeconds = 0;

    /** @return 普通命中或有限的完整物理冲击 */
    bool IsValid() const
    {
        if (Velocity.ContainsNaN() || !FMath::IsFinite(HoldSeconds) || HoldSeconds < 0 || HoldSeconds > 10
            || Velocity.SizeSquared() > FMath::Square(3000.0f))
        {
            return false;
        }
        return HoldSeconds == 0 ? Velocity.IsNearlyZero() : SourceId.IsValid() && AttackId > 0;
    }
};
