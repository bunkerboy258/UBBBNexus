#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/DomainData/States/BBBCharacterAppearanceInputState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

/** 角色外观的独立输入 */
struct FBBBCharacterAppearanceColorLocalControlPacket final
{
    /** 物品实例 为空时调整基础部件 */
    TArray<FGuid> Instances;
    /** 基础部件名称 */
    TArray<FName> Slots;
    /** 按区域排列的颜色 */
    TArray<TArray<FLinearColor>> Colors;
    /** @return 输入结构是否有效 */
    bool IsValid() const
    {
        if (Instances.IsEmpty() || Instances.Num() != Slots.Num() || Instances.Num() != Colors.Num())
        {
            return false;
        }
        for (int32 Index = 0; Index < Colors.Num(); ++Index)
        {
            if ((!Instances[Index].IsValid() && Slots[Index].IsNone()) || Colors[Index].Num() > 8)
            {
                return false;
            }
            for (const FLinearColor &Color : Colors[Index])
            {
                if (!FMath::IsFinite(Color.R) || !FMath::IsFinite(Color.G)
                    || !FMath::IsFinite(Color.B) || !FMath::IsFinite(Color.A))
                {
                    return false;
                }
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
        Context.AppearanceInputs.PendingColorInstances.Append(Instances);
        Context.AppearanceInputs.PendingColorSlots.Append(Slots);
        Context.AppearanceInputs.PendingColors.Append(Colors);
    }
};
