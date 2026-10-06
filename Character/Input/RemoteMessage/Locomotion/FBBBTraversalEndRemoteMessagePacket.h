#pragma once
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/DomainData/States/BBBCharacterTraversalState.h"

/** 翻越End结果输入 不进行几何或动画逻辑重演 */
struct FBBBTraversalEndRemoteMessagePacket final
{
    /** 来源控制者动作序号 */
    uint32 ActionId = 0;

    /** @return 输入基本值是否有效 */
    bool IsValid() const
    {
        return ActionId != 0;
    }
    /** @param Context 角色输入上下文 @return 是否接受该动作结果 */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return Context.bIsMirror && ActionId == Context.Traversal.ActionId
            && Context.Traversal.Action != EBBBTraversalAction::None;
    }
    /** @param Context 角色输入上下文 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.Traversal.bEndRequested = true;
    }
};
