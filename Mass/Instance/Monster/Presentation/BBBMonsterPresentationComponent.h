#pragma once

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehavior.h"
#include "Components/ActorComponent.h"

#include "BBBMonsterPresentationComponent.generated.h"

class UAnimSequenceBase;
class UBBBMonsterAnimInstance;

/** 向小怪动画蓝图提供只读 Mass 表现状态 */
UCLASS(ClassGroup = "Monster", BlueprintType, meta = (BlueprintSpawnableComponent))
class ABBB_EVAC_API UBBBMonsterPresentationComponent final : public UActorComponent
{
    GENERATED_BODY()

    friend class UBBBMonsterAnimInstance;
    friend class UBBBMonsterFactAnimInstance;

public:
    /** 创建小怪表现状态组件 */
    UBBBMonsterPresentationComponent();

    /**
     * 写入本帧由 Mass 逻辑生成的表现状态
     * @param InState        小怪权威状态
     * @param InSpeed        当前移动速度
     * @param InStateTime    状态进入世界时间
     */
    /**
     * 同状态动作通过独立编号重启 非循环动画读取逻辑进度
     * @param InState		小怪权威状态
     * @param InSpeed		当前移动速度
     * @param InStateTime	状态进入时间
     * @param InActionId		逻辑动作编号
     * @param InActionProgress	归一化动作位置
     * @return 无返回值
     */
    UFUNCTION()
    void ApplyPresentationState(
        EBBBMonsterBehavior InState,
        float InSpeed,
        float InStateTime,
        uint32 InActionId,
        float InActionProgress);

    /** @return 当前小怪状态 */
    UFUNCTION(BlueprintPure, Category = "小怪|表现")
    EBBBMonsterBehavior GetBBBMonsterBehavior() const;

    /** @return 当前移动速度 */
    UFUNCTION(BlueprintPure, Category = "小怪|表现")
    float GetMovementSpeed() const;

    /** @return 当前状态进入时间 */
    UFUNCTION(BlueprintPure, Category = "小怪|表现")
    float GetStateEnteredTime() const;

private:
    /**
     * 按稳定的表现种子和动作编号选择本次动作资产
     * @param InState		Mass 状态
     * @param InActionId		Mass 动作编号
     * @param InSeed		仅用于表现的稳定种子
     * @return 本次动画 无效配置返回空并报警
     */
    UAnimSequenceBase* SelectAnimationForAction(EBBBMonsterBehavior InState, uint32 InActionId, uint32 InSeed) const;

    /** 根据逻辑状态选择表现动画 */
    UAnimSequenceBase* GetAnimationForState(EBBBMonsterBehavior InState) const;

    /** 待机状态循环动画 */
    UPROPERTY(EditAnywhere, Category = "小怪|动画", meta = (DisplayName = "待机动画"))
    TObjectPtr<UAnimSequenceBase> IdleAnimation;

    /** 侦察状态循环动画 */
    UPROPERTY(EditAnywhere, Category = "小怪|动画", meta = (DisplayName = "侦察动画"))
    TObjectPtr<UAnimSequenceBase> ScoutAnimation;

    /** 追击状态循环动画 */
    UPROPERTY(EditAnywhere, Category = "小怪|动画", meta = (DisplayName = "追逐动画"))
    TObjectPtr<UAnimSequenceBase> ChaseAnimation;

    /** 攻击状态动画 */
    UPROPERTY(EditAnywhere, Category = "小怪|动画", meta = (DisplayName = "攻击动画"))
    TObjectPtr<UAnimSequenceBase> AttackAnimation;

    /** 受伤状态动画 */
    UPROPERTY(EditAnywhere, Category = "小怪|动画", meta = (DisplayName = "受击动画"))
    TObjectPtr<UAnimSequenceBase> HurtAnimation;

    /** 死亡状态动画 */
    UPROPERTY(EditAnywhere, Category = "小怪|动画", meta = (DisplayName = "死亡动画"))
    TObjectPtr<UAnimSequenceBase> DeadAnimation;

    /** 额外待机动画 与主要待机动画共同参与选择 */
    UPROPERTY(EditAnywhere, Category = "小怪|动画", meta = (DisplayName = "待机动画变体"))
    TArray<TObjectPtr<UAnimSequenceBase>> IdleAnimationVariants;

    /** 额外追击动画 与主要追击动画共同参与选择 */
    UPROPERTY(EditAnywhere, Category = "小怪|动画", meta = (DisplayName = "追逐动画变体"))
    TArray<TObjectPtr<UAnimSequenceBase>> ChaseAnimationVariants;

    /** 已裁剪为单次挥击的额外攻击动画 */
    UPROPERTY(EditAnywhere, Category = "小怪|动画", meta = (DisplayName = "攻击动画变体"))
    TArray<TObjectPtr<UAnimSequenceBase>> AttackAnimationVariants;

    /** 已裁剪为独立反应的额外受伤动画 */
    UPROPERTY(EditAnywhere, Category = "小怪|动画", meta = (DisplayName = "受击动画变体"))
    TArray<TObjectPtr<UAnimSequenceBase>> HurtAnimationVariants;

    /** 具有完整死亡过程的额外死亡动画 */
    UPROPERTY(EditAnywhere, Category = "小怪|动画", meta = (DisplayName = "死亡动画变体"))
    TArray<TObjectPtr<UAnimSequenceBase>> DeadAnimationVariants;

    /** 动作间双通道姿势过渡时长 */
    UPROPERTY(EditAnywhere, Category = "小怪|动画", meta = (ClampMin = "0.0", ClampMax = "0.5", Units = "s", DisplayName = "动画混合时间"))
    float AnimationBlendTime = 0.18f;

    /** 移动循环动画的标准速度 */
    UPROPERTY(EditAnywhere, Category = "小怪|动画", meta = (ClampMin = "1.0", Units = "cm/s", DisplayName = "动画参考速度"))
    float AnimationReferenceSpeed = 100.0f;

    /** Mass 提供的本次非循环动作归一化时间 */
    float ActionProgress = 0.0f;

    /** 当前逻辑状态 */
    UPROPERTY(Transient)
    EBBBMonsterBehavior BBBMonsterBehavior = EBBBMonsterBehavior::Idle;

    /** 当前移动速度 */
    UPROPERTY(Transient)
    float MovementSpeed = 0.0f;

    /** 当前状态进入时间 */
    UPROPERTY(Transient)
    float StateEnteredTime = 0.0f;

    /** 上一次已播放动画对应的状态 */
    EBBBMonsterBehavior LastPlayedState = EBBBMonsterBehavior::Idle;

    /** 是否已经向网格应用过动画 */
    bool bHasAppliedAnimation = false;

    /** 上一次已应用的逻辑动作编号 */
    uint32 LastActionId = 0;
};
