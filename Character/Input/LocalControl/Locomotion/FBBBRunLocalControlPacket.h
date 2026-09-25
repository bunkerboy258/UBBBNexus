#pragma once

#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

#include "CoreMinimal.h"

struct FBBBCharacterInputContext;

/** 跑步按键的持续意图输入 */
struct FBBBRunLocalControlPacket final
{
    /** 是否请求跑步 */
    bool bRun = false;

    /** @return 包内容是否合法 */
    bool IsValid() const
    {
        return true;
    }

    /** @param Context	本次输入上下文 @return 是否允许应用 */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return true;
    }

    /** @param Context	本次输入上下文 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.Control.bRun = bRun;
    }
};
