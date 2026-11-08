#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/DomainData/States/BBBCharacterAppearanceInputState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

/** 角色外观的独立输入 */
struct FBBBCharacterAppearanceBaseLocalControlPacket final
{
    /** 基础显示部件 */
    TArray<FName> Slots;
    /** 角色静态配置中的基础资源标识 */
    TArray<FName> Resources;
    /** @return 输入结构是否有效 */
    bool IsValid() const
    {
        if (Slots.IsEmpty() || Slots.Num() != Resources.Num())
        {
            return false;
        }
        for (int32 Index = 0; Index < Slots.Num(); ++Index)
        {
            if (Slots[Index].IsNone() || Resources[Index].IsNone())
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
        Context.AppearanceInputs.PendingBaseSlots.Append(Slots);
        Context.AppearanceInputs.PendingBaseResources.Append(Resources);
    }
};
