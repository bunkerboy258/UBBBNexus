#pragma once
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/TraversalSystem/DomainData/States/BBBCharacterTraversalState.h"

/** 翻越End结果输入 不进行几何或动画逻辑重演 */
struct FBBBTraversalEndRemoteMessagePacket final
{
    /** 来源控制者动作序号 */
    TArray<uint32> ActionIds;

    /** @return 输入基本值是否有效 */
    bool IsValid() const
    {
        return !ActionIds.IsEmpty() && !ActionIds.Contains(0);
    }
    /** @param Context 角色输入上下文 @return 是否接受该动作结果 */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        if (!Context.bIsMirror)
        {
            return false;
        }
        for (const uint32 Id : ActionIds)
        {
            if (int32(Id - Context.Traversal.ActionId) >= 0)
            {
                return true;
            }
        }
        return false;
    }
    /** @param Context 角色输入上下文 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        for (const uint32 Id : ActionIds)
        {
            if (int32(Id - Context.Traversal.ActionId) >= 0)
            {
                Context.Traversal.ActionId = Id;
                Context.Traversal.bEndRequested = true;
            }
        }
    }
};
