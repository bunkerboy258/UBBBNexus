#pragma once

#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/States/BBBCharacterAnimationMontageState.h"

#include "CoreMinimal.h"

class UAnimMontage;
struct FBBBCharacterInputContext;

/** FullBodyAdditivePreAim 固定槽位的蒙太奇贡献 */
struct FBBBFullBodyAdditivePreAimMontageLocalControlPacket final
{
    /** 待播放蒙太奇 空引用表示清除此槽 */
    TObjectPtr<UAnimMontage> Montage = nullptr;

    /** @return 输入是否有效 */
    bool IsValid() const
    {
        return true;
    }

    /** @param Context	角色解析上下文 @return 是否允许贡献到固定槽位 */
    bool CanApply(const FBBBCharacterInputContext &) const
    {
        return true;
    }

    /** @param Context	角色解析上下文 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.AnimationMontageState.FullBodyAdditivePreAimMontageRequest = Montage.Get();
        Context.AnimationMontageState.bFullBodyAdditivePreAimMontageRequestPending = true;
    }
};
