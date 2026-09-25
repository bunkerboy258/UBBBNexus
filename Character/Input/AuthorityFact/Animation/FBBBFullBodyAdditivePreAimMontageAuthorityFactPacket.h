#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/States/BBBCharacterAnimationMontageState.h"

class UAnimMontage;

/** 权威事实还原的 FullBodyAdditivePreAim 蒙太奇槽输入 */
struct FBBBFullBodyAdditivePreAimMontageAuthorityFactPacket final
{
    /** 空引用表示清除此槽 */
    TObjectPtr<UAnimMontage> Montage = nullptr;

    /** @return 输入数据是否合法 */
    bool IsValid() const
    {
        return true;
    }

    /**
     * 检查是否允许还原槽位
     * @param Context	角色黑板上下文
     * @return 是否允许应用
     */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return true;
    }

    /**
     * 写入待消费的槽位结果
     * @param Context	角色黑板上下文
     * @return 无
     */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.AnimationMontageState.FullBodyAdditivePreAimMontageRequest = Montage.Get();
        Context.AnimationMontageState.bFullBodyAdditivePreAimMontageRequestPending = true;
    }
};
