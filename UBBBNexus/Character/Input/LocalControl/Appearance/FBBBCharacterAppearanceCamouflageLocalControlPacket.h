#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/DomainData/States/BBBCharacterAppearanceInputState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

/** 角色外观的独立输入 */
struct FBBBCharacterAppearanceCamouflageLocalControlPacket final
{
    /** 物品实例 为空时调整基础部件 */
    TArray<FGuid> Instances;
    /** 基础部件名称 */
    TArray<FName> Slots;
    /** 迷彩开关 */
    TArray<bool> Values;
    /** @return 输入结构是否有效 */
    bool IsValid() const
    {
        if (Instances.IsEmpty() || Instances.Num() != Slots.Num() || Instances.Num() != Values.Num())
        {
            return false;
        }
        for (int32 Index = 0; Index < Instances.Num(); ++Index)
        {
            if (!Instances[Index].IsValid() && Slots[Index].IsNone())
            {
                return false;
            }
        }
        return true;
    }
    /** @param Context 角色输入上下文 @return 是否允许本路径消费 */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return !Context.bIsMirror;
    }
    /** @param Context 角色输入上下文 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.AppearanceInputs.PendingCamouflageInstances.Append(Instances);
        Context.AppearanceInputs.PendingCamouflageSlots.Append(Slots);
        Context.AppearanceInputs.PendingCamouflage.Append(Values);
    }
};
