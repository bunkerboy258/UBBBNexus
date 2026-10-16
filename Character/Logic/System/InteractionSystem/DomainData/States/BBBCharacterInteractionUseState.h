#pragma once
#include "CoreMinimal.h"
#include "BBBCharacterInteractionUseState.generated.h"

/** 当前持有物品的交互权限与通知标识 */
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
};
