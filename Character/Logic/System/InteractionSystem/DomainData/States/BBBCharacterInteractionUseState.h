#pragma once
#include "CoreMinimal.h"
#include "BBBCharacterInteractionUseState.generated.h"

/** 持有关系不变时独立维护装备使用与临时收起 */
USTRUCT()
struct FBBBCharacterInteractionUseState final
{
    GENERATED_BODY()
    /** 当前动作是否阻止背包操作与主动切换 */
    bool bBusy = false;
    /** 当前实例的只读使用序号 用于拒绝过期通知 */
    int32 UseSerial = 0;
    /** 当前持有物品是否允许主动切换或解除装备 */
    bool bSwitchAllowed = true;
    /** 当前持有物品是否允许 G 快捷消耗品打断 */
    bool bQuickUseAllowed = true;
    /** 当前持有实例是否可使用 */
    bool bUsable = false;
    /** 是否已建立当前实例的使用基线 */
    bool bInitialized = false;
    /** 使用状态对应的持有实例 */
    uint64 Generation = 0;
    /** 已成立的使用状态版本 */
    uint64 Revision = 0;
};
