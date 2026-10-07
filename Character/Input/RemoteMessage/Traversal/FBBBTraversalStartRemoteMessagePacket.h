#pragma once
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/TraversalSystem/DomainData/States/BBBCharacterTraversalState.h"

/** 翻越Start结果输入 不进行几何或动画逻辑重演 */
struct FBBBTraversalStartRemoteMessagePacket final
{
    /** 来源控制者动作序号 */
    TArray<uint32> ActionIds;
    /** 已经确定的动作类别 */
    TArray<EBBBTraversalAction> Actions;
    /** 前沿世界目标 */
    TArray<FTransform> Contacts;
    /** 脚底世界目标 */
    TArray<FTransform> Ends;

    /** 当前已经成立的动画进度用于恢复当前表现 */
    TArray<float> Positions;

    /** @return 输入基本值是否有效 */
    bool IsValid() const
    {
        if (ActionIds.IsEmpty() || Actions.Num() != ActionIds.Num() || Contacts.Num() != ActionIds.Num()
            || Ends.Num() != ActionIds.Num() || Positions.Num() != ActionIds.Num())
        {
            return false;
        }
        for (int32 Index = 0; Index < ActionIds.Num(); ++Index)
        {
            if (ActionIds[Index] == 0 || Actions[Index] <= EBBBTraversalAction::None
                || Actions[Index] > EBBBTraversalAction::ClimbHigh || !Contacts[Index].IsValid() || !Ends[Index].IsValid()
                || !FMath::IsFinite(Positions[Index]) || Positions[Index] < 0.0f)
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
            if (int32(Id - Context.Traversal.ActionId) > 0)
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
            if (int32(ActionIds[Index] - Context.Traversal.ActionId) <= 0)
            {
                continue;
            }
            Context.Traversal.ActionId = ActionIds[Index];
            Context.Traversal.Action = Actions[Index];
            Context.Traversal.ContactTarget = Contacts[Index];
            Context.Traversal.EndTarget = Ends[Index];
            Context.Traversal.bEndRequested = false;
            Context.Traversal.bAnimationReleased = false;
            Context.Traversal.PlaybackPosition = Positions[Index];
            Context.Traversal.bPlaybackRequested = true;
            Context.Traversal.bPlaybackObserved = false;
        }
    }
};
