#pragma once

#include "CoreMinimal.h"

/** 角色额外瞄准冲击及回正状态 */
struct FBBBCharacterAimImpulseState final
{
    /** 待消费的上下与左右角度增量 */
    FVector2D PendingDegrees = FVector2D::ZeroVector;

    /** 当前上下与左右角度偏移 */
    FVector2D OffsetDegrees = FVector2D::ZeroVector;

private:
    friend struct FBBBCharacterAnimationDomainState;

    FBBBCharacterAimImpulseState() = default;
    FBBBCharacterAimImpulseState(const FBBBCharacterAimImpulseState &) = default;
    FBBBCharacterAimImpulseState &operator=(const FBBBCharacterAimImpulseState &) = default;
};
