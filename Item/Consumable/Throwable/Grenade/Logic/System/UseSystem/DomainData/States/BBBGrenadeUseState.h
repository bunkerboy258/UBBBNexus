#pragma once
#include "CoreMinimal.h"
/** 当次使用的动作状态 */
struct FBBBGrenadeUseState final
{
    /** 是否仍占用本次使用 */
    bool bUsing = false;
    /** 已贡献的动作阶段 */
    uint8 Stage = 255;

  private:
    friend struct FBBBGrenadeUseDomainState;
    FBBBGrenadeUseState() = default;
};
