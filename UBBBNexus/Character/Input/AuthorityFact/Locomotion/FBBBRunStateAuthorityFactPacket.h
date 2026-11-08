#pragma once

#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

#include "CoreMinimal.h"

struct FBBBCharacterInputContext;

/** 镜像角色接收的当前跑步状态 */
struct FBBBRunStateAuthorityFactPacket final
{
    /** 是否正在跑步 */
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
        Context.Locomotion.bRun = bRun;
    }
};
