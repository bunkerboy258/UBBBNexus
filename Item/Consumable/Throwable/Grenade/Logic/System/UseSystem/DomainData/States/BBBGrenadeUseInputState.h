#pragma once
#include "CoreMinimal.h"
/** 当帧已经批准的动作请求 */
struct FBBBGrenadeUseInputState final
{
    /** 是否存在动作请求 */
    bool bPrimary = false;
    /** 是否结束当次使用 */
    bool bFinish = false;

  private:
    friend struct FBBBGrenadeUseDomainState;
    FBBBGrenadeUseInputState() = default;
};
