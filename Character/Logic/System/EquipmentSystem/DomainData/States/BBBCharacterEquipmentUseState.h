#pragma once
#include "CoreMinimal.h"
#include "BBBCharacterEquipmentUseState.generated.h"

/** 持有关系不变时独立维护装备使用与临时收起 */
USTRUCT()
struct FBBBCharacterEquipmentUseState final
{
    GENERATED_BODY()
    /** 当前持有实例是否可使用 */
    bool bUsable = false;
    /** 是否已建立当前实例的使用基线 */
    bool bInitialized = false;
    /** 使用状态对应的持有实例 */
    uint64 Generation = 0;
    /** 已成立的使用状态版本 */
    uint64 Revision = 0;
};
