#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ItemSystem/DomainData/States/BBBCharacterItemOperationState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

/** 物品已经使用成功后的通用数量扣减 */
struct FBBBCharacterItemConsumeLocalControlPacket final
{
    /** 要扣减的真实物品身份 */
    TArray<FGuid> Instances;
    /** 对应的扣减数量 */
    TArray<int32> Quantities;
    /** @return 输入格式是否有效 */
    bool IsValid() const
    {
        if (Instances.IsEmpty() || Instances.Num() != Quantities.Num())
        {
            return false;
        }
        for (int32 Index = 0; Index < Instances.Num(); ++Index)
        {
            if (!Instances[Index].IsValid() || Quantities[Index] <= 0)
            {
                return false;
            }
        }
        return true;
    }
    /** @param Context	角色输入上下文 @return 是否维护真实物品 */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return !Context.bIsMirror;
    }
    /** @param Context	角色输入上下文 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.ItemOperations.PendingConsumeInstances.Append(Instances);
        Context.ItemOperations.PendingConsumeQuantities.Append(Quantities);
    }
};
