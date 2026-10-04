#pragma once

#include "Animation/AnimInstance.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehavior.h"

#include "BBBMonsterAnimInstance.generated.h"

class UAnimSequenceBase;
class UBBBMonsterPresentationComponent;

/** 读取 Mass 表现快照并向双通道动画图提供无根运动的显式采样 */
UCLASS(Transient, Blueprintable)
class ABBB_EVAC_API UBBBMonsterAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    //~ Begin UAnimInstance Interface
    /** @return 初始化本演员的动画只读桥接 无返回值 */
    virtual void NativeInitializeAnimation() override;

    /**
     * 将最新 Mass 快照转换为动画求值参数
     * @param DeltaSeconds	引擎动画更新间隔 不推进非循环玩法动作
     * @return 无返回值
     */
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;
    //~ End UAnimInstance Interface

    /** @return 通道甲当前动画 */
    UFUNCTION(BlueprintPure, Category = "Monster|Animation", meta = (BlueprintThreadSafe))
    UAnimSequenceBase* GetChannelAAnimation() const;

    /** @return 通道乙当前动画 */
    UFUNCTION(BlueprintPure, Category = "Monster|Animation", meta = (BlueprintThreadSafe))
    UAnimSequenceBase* GetChannelBAnimation() const;

    /** @return 通道甲显式采样时间 */
    UFUNCTION(BlueprintPure, Category = "Monster|Animation", meta = (BlueprintThreadSafe))
    float GetChannelATime() const;

    /** @return 通道乙显式采样时间 */
    UFUNCTION(BlueprintPure, Category = "Monster|Animation", meta = (BlueprintThreadSafe))
    float GetChannelBTime() const;

    /** @return 当前是否使用通道乙 */
    UFUNCTION(BlueprintPure, Category = "Monster|Animation", meta = (BlueprintThreadSafe))
    bool IsChannelBActive() const;

    /** @return 资产配置的动作过渡时长 */
    UFUNCTION(BlueprintPure, Category = "Monster|Animation", meta = (BlueprintThreadSafe))
    float GetTransitionBlendTime() const;

    /** @return 当前动作动画 */
    UFUNCTION(BlueprintPure, Category = "Monster|Animation", meta = (BlueprintThreadSafe))
    UAnimSequenceBase* GetActiveAnimation() const;

    /** @return 当前动作显式采样时间 */
    UFUNCTION(BlueprintPure, Category = "Monster|Animation", meta = (BlueprintThreadSafe))
    float GetActiveAnimationTime() const;

private:
    /** 所属表现组件的弱引用 不持有玩法状态 */
    UPROPERTY(Transient)
    TWeakObjectPtr<UBBBMonsterPresentationComponent> Presentation;

    /** 通道甲保持过渡前的动画引用 */
    UPROPERTY(Transient)
    TObjectPtr<UAnimSequenceBase> ChannelAAnimation;

    /** 通道乙保持过渡前的动画引用 */
    UPROPERTY(Transient)
    TObjectPtr<UAnimSequenceBase> ChannelBAnimation;

    /** 通道甲只读动画采样时间 */
    float ChannelATime = 0.0f;

    /** 通道乙只读动画采样时间 */
    float ChannelBTime = 0.0f;

    /** 当前显示通道 */
    bool bChannelBActive = false;

    /** 快照初始化标记 */
    bool bHasSnapshot = false;

    /** 仅用于识别新动作的上次行为类型 */
    EBBBMonsterBehavior LastBehavior = EBBBMonsterBehavior::Idle;

    /** 仅用于识别新动作的上次动作编号 */
    uint32 LastActionId = 0;

    /** 仅用于识别新动作的上次进入时间 */
    float LastStateEnteredTime = 0.0f;

    /** 本帧由表现资产配置复制的过渡时长 */
    float TransitionBlendTime = 0.18f;
};
