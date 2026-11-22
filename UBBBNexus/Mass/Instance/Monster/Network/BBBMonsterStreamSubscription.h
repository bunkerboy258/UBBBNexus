#pragma once

#include "CoreMinimal.h"

/** 一个连接已排入传输的当前实体进度 */
struct FBBBMonsterStreamSubscription final
{
    uint64 DamageRevision = 0;
    uint64 InitialDamageThrough = 0;
    uint32 DeliverySequence = 0;
    uint32 BirthSequence = 0;
    uint32 MotionRevision = 0;
    double NextMotionAt = 0.0;
    double LastMotionAt = -10.0;
    bool bSentDeathPose = false;
};
