#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/EquipmentSystem/DomainData/States/BBBCharacterEquipmentSelectionState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

/** 权威事实还原的当前装备选择输入 */
struct FBBBEquipmentSelectionAuthorityFactPacket final
{
    /** 空标识表示空手 */
    FName EquipmentId = NAME_None;

    /** @return 输入数据是否合法 */
    bool IsValid() const
    {
        return true;
    }

    /**
     * 检查是否允许还原装备选择
     * @param Context	角色黑板上下文
     * @return 是否允许应用
     */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return true;
    }

    /**
     * 写入待消费的装备选择结果
     * @param Context	角色黑板上下文
     * @return 无
     */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.EquipmentSelection.PendingEquipmentId = EquipmentId;
        Context.EquipmentSelection.bHasEquipmentRequest = true;
    }
};
