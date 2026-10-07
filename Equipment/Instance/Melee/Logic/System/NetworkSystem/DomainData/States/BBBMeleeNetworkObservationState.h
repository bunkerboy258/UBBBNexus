#pragma once

#include "CoreMinimal.h"

/** 近战已经发送的当前事实基准 */
struct FBBBMeleeNetworkObservationState final
{
    /** 已发送的持有实例 */
    uint64 Generation = 0;

    /** 已发布本次持有的基准 */
    bool bPublished = false;

    /** 已发送的AttackSequence事实 */
    int32 AttackSequence = 0;

    /** 已发送的bAttacking事实 */
    bool bAttacking = false;

private:
    friend struct FBBBMeleeNetworkDomainState;
    FBBBMeleeNetworkObservationState() = default;
};
