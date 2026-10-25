#pragma once

#include "BBBArmDamageDomainState.h"

/** 独立左臂样板持有的唯一损伤黑板 */
struct FBBBArmDamageRuntimeData final
{
    /** 当前组织损伤领域 */
    FBBBArmDamageDomainState Damage;
};
