#pragma once
#include "CoreMinimal.h"

/** 待还原的当前装备使用结果 */
struct FBBBCharacterEquipmentUseInputState final
{
    /** 结果所属持有实例 */
    TArray<uint64> Generations;
    /** 同一角色生命周期中的结果版本 */
    TArray<uint64> Revisions;
    /** 对应实例是否可使用 */
    TArray<bool> Usable;
};
