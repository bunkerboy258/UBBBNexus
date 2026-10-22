#pragma once

#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

/** 将输入适配层已有开火按住状态交给角色表现 不触发武器动作 */
struct FBBBCharacterFireIntentLocalControlPacket final
{
    /** 已有输入系统的开火意图值 */
    bool bFire = false;

    /** @return 布尔表现输入始终有效 */
    bool IsValid() const
    {
        return true;
    }

    /** @param Context	当前角色输入上下文 @return 是否由本机输入提供表现状态 */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return !Context.bIsMirror;
    }

    /** @param Context	当前角色输入上下文 @return 无 只发布表现用控制快照 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.Control.bFire = bFire;
    }
};
