#pragma once
#include "CoreMinimal.h"

/** 当前玩法固定的有向冲突规则 */
struct FBBBCharacterConflictRules
{
    /** @return 当前是否允许发起开火 */
    static bool AllowsFire(bool bReloading, bool bSwitching, bool bStartingReload);
    /** @return 当前是否允许冲刺 */
    static bool AllowsSprint(bool bAiming);
};
