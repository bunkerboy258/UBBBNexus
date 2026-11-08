#pragma once
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/LifeSystem/DomainData/States/BBBCharacterLifeInputState.h"
/** 通知已经成立的恢复量 */
struct FBBBCharacterRecoveryLocalControlPacket final
{
    /** 同帧恢复请求 */
    TArray<float> Amounts;
    /** @return 格式有效 */
    bool IsValid() const
    {
        for (float Amount : Amounts)
        {
            if (!FMath::IsFinite(Amount) || Amount <= 0.0f)
            {
                return false;
            }
        }
        return !Amounts.IsEmpty();
    }
    /** @param Context 解析上下文 @return 是否允许恢复 */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return !Context.bIsMirror && Context.Life.Phase == EBBBCharacterLifePhase::Alive;
    }
    /** @param Context 输入状态 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.LifeInputs.Recoveries.Append(Amounts);
    }
};
