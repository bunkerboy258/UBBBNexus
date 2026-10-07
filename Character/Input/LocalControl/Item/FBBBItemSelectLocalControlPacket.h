#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/TraversalSystem/DomainData/States/BBBCharacterTraversalState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/DomainData/States/BBBCharacterItemOperationState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

/** 本机物品快捷选择输入 */
struct FBBBItemSelectLocalControlPacket final
{
    /** 待选择的前序槽位 INDEX_NONE 表示取消选择 */
    TArray<int32> Slots;

    /** @return 输入数据格式是否有效 */
    bool IsValid() const
    {
        if (Slots.IsEmpty())
        {
            return false;
        }
        for (const int32 Slot : Slots)
        {
            if (Slot < INDEX_NONE)
            {
                return false;
            }
        }
        return true;
    }

    /** @param Context	本次输入上下文 @return 当前角色是否拥有真实物品控制权 */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return Context.Life.Phase == EBBBCharacterLifePhase::Alive
            && Context.Traversal.Action == EBBBTraversalAction::None && !Context.bIsMirror;
    }

    /** @param Context	本次输入上下文 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.ItemOperations.PendingSelectedSlots.Append(Slots);
    }
};
