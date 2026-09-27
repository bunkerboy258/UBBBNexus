#pragma once

#include "MassEntityTypes.h"
#include "BBBMonsterDamageFragment.generated.h"

/** 解析结果交给健康处理器消费 */
USTRUCT()
struct ABBB_EVAC_API FBBBMonsterDamageFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 本帧本机有效命中伤害 */
    float PendingDamage = 0.0f;

    /** 待合并的远端剩余血量 未提交时不降低当前血量 */
    float ReportedHealth = TNumericLimits<float>::Max();

    /** 已扣血的受伤事实 */
    bool bReceivedDamage = false;
};
