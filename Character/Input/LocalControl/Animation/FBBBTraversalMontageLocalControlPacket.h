#pragma once

#include "CoreMinimal.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/ParseSystem/DomainData/Context/BBBCharacterInputContext.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/AnimationSystem/DomainData/States/BBBCharacterAnimationMontageState.h"
#include "BBBWork/UBBBNexus/Character/Logic/System/TraversalSystem/DomainData/States/BBBCharacterTraversalState.h"
#include "FBBBTraversalMontageLocalControlPacket.generated.h"

class UAnimMontage;

/** 攀爬状态专用槽位的蒙太奇贡献 */
USTRUCT(BlueprintType)
struct FBBBTraversalMontageLocalControlPacket final
{
    GENERATED_BODY()

    /** 已确认动作所使用的蒙太奇 空引用清除此槽 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "动画")
    TObjectPtr<UAnimMontage> Montage = nullptr;

    /** @return 空引用可表达清除请求 */
    bool IsValid() const
    {
        return true;
    }

    /** @param Context 角色解析上下文 @return 当前动作是否允许贡献攀爬动画 */
    bool CanApply(const FBBBCharacterInputContext &Context) const
    {
        return !Montage || (Context.Life.Phase == EBBBCharacterLifePhase::Alive
            && Context.Traversal.Action != EBBBTraversalAction::None && !Context.Traversal.bEndRequested);
    }

    /** @param Context 角色解析上下文 @return 无 */
    void Apply(FBBBCharacterInputContext &Context) const
    {
        Context.AnimationMontageState.TraversalMontageRequest = Montage.Get();
        Context.AnimationMontageState.bTraversalMontageRequestPending = true;
    }
};
