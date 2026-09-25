#pragma once

#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/DomainData/States/BBBCharacterEquipmentInventoryState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/DomainData/States/BBBCharacterEquipmentSelectionState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

#include "CoreMinimal.h"

struct FBBBCharacterInputContext;

/** 本机快捷栏装备选择请求 */
struct FBBBEquipmentSlotLocalControlPacket final
{
    /** 空手使用 INDEX_NONE */
    int32 Slot = INDEX_NONE;

    /** @return 槽位格式是否合法 */
    bool IsValid() const
    {
        return Slot >= INDEX_NONE;
    }

    /** @param Context	本次输入上下文 @return 槽位是否存在 */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return Slot == INDEX_NONE || Context.EquipmentInventory.ItemBarSlots.IsValidIndex(Slot);
    }

    /** @param Context	本次输入上下文 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.EquipmentSelection.PendingSlot = Slot;
    }
};
