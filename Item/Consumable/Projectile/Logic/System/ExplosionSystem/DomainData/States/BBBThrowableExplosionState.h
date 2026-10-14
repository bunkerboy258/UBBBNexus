#pragma once
#include "BBBWork/UBBBNexus/Item/Consumable/Projectile/Network/BBBThrowableExplosionFact.h"
/** 单个投掷物的爆炸业务状态 */
struct FBBBThrowableExplosionState final
{
    /** 是否有待处理的引爆请求 */
    bool bRequested = false;
    /** 终态表现是否已还原 */
    bool bPresented = false;
    /** 当前唯一的爆炸结果 */
    FBBBThrowableExplosionFact Fact;
    /** 已交接伤害的终态目标索引 */
    TSet<int32> Credited;
};
