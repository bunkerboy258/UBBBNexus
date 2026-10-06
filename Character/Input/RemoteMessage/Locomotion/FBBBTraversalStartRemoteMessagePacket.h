#pragma once
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/DomainData/States/BBBCharacterTraversalState.h"

/** 翻越Start结果输入 不进行几何或动画逻辑重演 */
struct FBBBTraversalStartRemoteMessagePacket final
{
    /** 来源控制者动作序号 */
    uint32 ActionId = 0;
    /** 已经确定的动作类别 */
    EBBBTraversalAction Action = EBBBTraversalAction::None;
    /** 前沿世界目标 */
    FTransform Contact = FTransform::Identity;
    /** 脚底世界目标 */
    FTransform End = FTransform::Identity;

    /** @return 输入基本值是否有效 */
    bool IsValid() const
    {
        return ActionId != 0 && Action > EBBBTraversalAction::None
            && Action <= EBBBTraversalAction::ClimbHigh && Contact.IsValid() && End.IsValid();
    }
    /** @param Context 角色输入上下文 @return 是否接受该动作结果 */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return Context.bIsMirror && int32(ActionId - Context.Traversal.ActionId) > 0;
    }
    /** @param Context 角色输入上下文 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.Traversal.ActionId = ActionId;
        Context.Traversal.Action = Action;
        Context.Traversal.ContactTarget = Contact;
        Context.Traversal.EndTarget = End;
        Context.Traversal.bEndRequested = false;
        Context.Traversal.bPlaybackRequested = false;
        Context.Traversal.bPlaybackObserved = false;
    }
};
