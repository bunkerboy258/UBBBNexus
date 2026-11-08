#pragma once
#include "CoreMinimal.h"
/** 当次使用的动作状态 */
struct FBBBHealthPackUseState final
{
    /** 是否仍占用本次使用 */
    bool bUsing = false;

  private:
    friend struct FBBBHealthPackUseDomainState;
    FBBBHealthPackUseState() = default;
};
