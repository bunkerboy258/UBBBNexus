#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/DomainData/States/BBBCharacterItemOperationState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

/** 本机物品移动交换输入 */
struct FBBBItemMoveLocalControlPacket final
{
    /** 起始槽位 与目标数组一一对应 */
    TArray<int32> Sources;

    /** 目标槽位 */
    TArray<int32> Targets;

    /** @return 输入数据格式是否有效 */
    bool IsValid() const
    {
        if (Sources.IsEmpty() || Sources.Num() != Targets.Num())
        {
            return false;
        }
        for (int32 Index = 0; Index < Sources.Num(); ++Index)
        {
            if (Sources[Index] < 0 || Targets[Index] < 0)
            {
                return false;
            }
        }
        return true;
    }

    /** @param Context	本次输入上下文 @return 当前角色是否拥有真实物品控制权 */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return !Context.bIsMirror && (Context.Life.bActionsAllowed && !Context.RescueInputs.bBegin);
    }

    /** @param Context	本次输入上下文 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.ItemOperations.PendingMoveSources.Append(Sources);
        Context.ItemOperations.PendingMoveTargets.Append(Targets);
    }
};
