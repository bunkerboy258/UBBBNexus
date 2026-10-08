#pragma once
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/TraversalSystem/DomainData/States/BBBCharacterTraversalState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LocomotionSystem/DomainData/States/BBBCharacterLocomotionState.h"

/** 翻越End结果输入 不进行几何或动画逻辑重演 */
struct FBBBTraversalEndAuthorityFactPacket final
{
    /** 来源控制者动作序号 */
    TArray<uint32> ActionIds;

    /** 与动作序号对应的既成交权速度 不通过镜像输入重新计算 */
    TArray<FVector> ExitVelocities;

    /** @return 输入基本值是否有效 */
    bool IsValid() const
    {
        if (ActionIds.IsEmpty() || ActionIds.Contains(0) || ActionIds.Num() != ExitVelocities.Num())
        {
            return false;
        }
        for (const FVector &Velocity : ExitVelocities)
        {
            if (Velocity.ContainsNaN() || Velocity.GetAbsMax() > 10000.0)
            {
                return false;
            }
        }
        return true;
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
        for (int32 Index = 0; Index < ActionIds.Num(); ++Index)
        {
            const uint32 Id = ActionIds[Index];
            if (int32(Id - Context.Traversal.ActionId) >= 0)
            {
                Context.Traversal.ActionId = Id;
                Context.Traversal.bEndRequested = true;
                Context.Locomotion.TraversalExitVelocity = ExitVelocities[Index];
                Context.Locomotion.bTraversalExitPrepared = true;
            }
        }
    }
};
