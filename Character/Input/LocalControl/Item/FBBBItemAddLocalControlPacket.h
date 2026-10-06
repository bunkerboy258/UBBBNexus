#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/DomainData/States/BBBCharacterItemOperationState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

/** 本机物品获得输入 */
struct FBBBItemAddLocalControlPacket final
{
    /** 待获得的装备定义 按提交顺序累计 */
    TArray<FName> EquipmentIds;

    /** @return 输入数据格式是否有效 */
    bool IsValid() const
    {
        if (EquipmentIds.IsEmpty())
        {
            return false;
        }
        for (const FName Id : EquipmentIds)
        {
            if (Id.IsNone())
            {
                return false;
            }
        }
        return true;
    }

    /** @param Context	本次输入上下文 @return 当前角色是否拥有真实物品控制权 */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return !Context.bIsMirror;
    }

    /** @param Context	本次输入上下文 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.ItemOperations.PendingEquipmentIds.Append(EquipmentIds);
    }
};
