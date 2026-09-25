#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

/**
 * 跳跃请求包 与装备动作无冲突
 */
struct FBBBJumpLocalControlPacket final
{

    /** @return 包内容是否合法 */
    bool IsValid() const
    {
        return true;
    }

    /**
     * 检查本帧是否允许执行
     * @param Context	黑板上下文
     * @return 是否允许执行
     */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return true;
    }

    /**
     * 登记跳跃意图到控制事实
     * @param Context	黑板上下文
     */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.Control.bJump = true;
    }
};
