#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/DomainData/States/BBBCharacterItemOperationState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

/** 按全服持久快照精确还原角色唯一背包与穿戴位置 */
struct FBBBCharacterItemRestoreLocalControlPacket final
{
    /** 待还原槽位索引 */
    TArray<int32> SlotIndices;
    /** 待还原物品实例身份 */
    TArray<FGuid> InstanceIds;
    /** 待还原物品型号标识 */
    TArray<FName> ItemIds;
    /** 待还原物品堆叠数量 */
    TArray<int32> Quantities;
    /** 还原后的装备快捷选中索引 INDEX_NONE 表示空手 */
    int32 SelectedWeaponSlot = INDEX_NONE;
    /** 还原后的消耗品快捷选中索引 */
    int32 SelectedConsumableSlot = INDEX_NONE;

    /** @return 还原快照结构是否有效 */
    bool IsValid() const
    {
        if (SlotIndices.Num() != InstanceIds.Num()
            || SlotIndices.Num() != ItemIds.Num()
            || SlotIndices.Num() != Quantities.Num()
            || SlotIndices.Num() > 256)
        {
            return false;
        }
        for (int32 Index = 0; Index < SlotIndices.Num(); ++Index)
        {
            if (SlotIndices[Index] < 0 || ItemIds[Index].IsNone()
                || Quantities[Index] <= 0 || Quantities[Index] > 1000000)
            {
                return false;
            }
        }
        return true;
    }

    /** @param Context	角色输入上下文 @return 是否拥有真实物品权限 */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return !Context.bIsMirror;
    }

    /** @param Context	角色输入上下文 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.ItemOperations.bPendingRestore = true;
        Context.ItemOperations.PendingRestoreSlots = SlotIndices;
        Context.ItemOperations.PendingRestoreInstances = InstanceIds;
        Context.ItemOperations.PendingRestoreItemIds = ItemIds;
        Context.ItemOperations.PendingRestoreQuantities = Quantities;
        Context.ItemOperations.PendingRestoreWeaponSlot = SelectedWeaponSlot;
        Context.ItemOperations.PendingRestoreConsumableSlot = SelectedConsumableSlot;
    }
};
