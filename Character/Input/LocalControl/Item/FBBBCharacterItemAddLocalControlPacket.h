#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/DomainData/States/BBBCharacterItemOperationState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

/** 角色获得物品的整次数量请求 */
struct FBBBCharacterItemAddLocalControlPacket final
{
    /** 待获得的型号 */
    TArray<FName> ItemIds;
    /** 各型号对应的数量 */
    TArray<int32> Quantities;
    /** @return 请求结构是否有效 */
    bool IsValid() const
    {
        if (ItemIds.IsEmpty() || ItemIds.Num() != Quantities.Num())
        {
            return false;
        }
        for (int32 Index = 0; Index < ItemIds.Num(); ++Index)
        {
            if (ItemIds[Index].IsNone() || Quantities[Index] <= 0 || Quantities[Index] > 1000000)
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
        Context.ItemOperations.PendingItemIds.Append(ItemIds);
        Context.ItemOperations.PendingQuantities.Append(Quantities);
    }
};
