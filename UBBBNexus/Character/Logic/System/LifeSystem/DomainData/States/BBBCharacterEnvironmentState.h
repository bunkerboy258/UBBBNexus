#pragma once

#include "CoreMinimal.h"

/** 本机环境接触采样与尚未递交的持续伤害 */
struct FBBBCharacterEnvironmentState final
{
    /** 下一次环境空间采样时间 */
    double NextContactAt = 0;
    /** 下一次持续伤害递交时间 */
    double NextDamageAt = 0;
    /** 当前采样的每秒伤害 */
    float DamagePerSecond = 0;
    /** 已接触期间累计的伤害 */
    float PendingDamage = 0;
};
