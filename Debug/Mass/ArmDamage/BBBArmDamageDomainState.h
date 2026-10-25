#pragma once

#include "BBBArmDamageState.h"

/** 独立左臂损伤状态的唯一直接持有者 */
struct FBBBArmDamageDomainState final
{
    /** @return 当前左臂累计损伤与连接结果 */
    const FBBBArmDamageState& ReadArmDamageState() const
    {
        return ArmDamage;
    }

private:
    friend struct FBBBArmDamageProcessor;
    FBBBArmDamageState ArmDamage;
};
