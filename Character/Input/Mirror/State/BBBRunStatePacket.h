#pragma once

#include "CoreMinimal.h"

struct FBBBCharacterInputContext;

/** 镜像角色接收的当前跑步状态 */
struct FBBBRunStatePacket final
{
    /** 是否正在跑步 */
    bool bRun = false;

    /** @return 包内容是否合法 */
    bool IsValid() const;

    /** @param Context	本次输入上下文 @return 是否允许应用 */
    bool CanApply(const FBBBCharacterInputContext &Context) const;

    /** @param Context	本次输入上下文 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const;
};
