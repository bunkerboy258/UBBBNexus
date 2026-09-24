#pragma once

#include "CoreMinimal.h"

struct FBBBCharacterInputContext;

/** 蹲伏按键的持续意图输入 */
struct FBBBCrouchPacket final
{
    /** 是否请求蹲伏 */
    bool bCrouch = false;

    /** @return 包内容是否合法 */
    bool IsValid() const;

    /** @param Context	本次输入上下文 @return 是否允许应用 */
    bool CanApply(const FBBBCharacterInputContext &Context) const;

    /** @param Context	本次输入上下文 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const;
};
