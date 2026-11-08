#pragma once
#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AppearanceSystem/DomainData/States/BBBCharacterAppearanceInputState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"

/** 角色外观的独立输入 */
struct FBBBCharacterAppearanceWeatheringLocalControlPacket final
{
    /** 整体强度 */
    TArray<float> Values;
    /** @return 输入结构是否有效 */
    bool IsValid() const
    {
        if (Values.IsEmpty())
        {
            return false;
        }
        for (const float Value : Values)
        {
            if (!FMath::IsFinite(Value) || Value < 0.0f || Value > 1.0f)
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
        Context.AppearanceInputs.PendingWeathering.Append(Values);
    }
};
