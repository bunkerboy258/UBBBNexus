#pragma once

#include "Components/ActorComponent.h"
#include "BBBWork/UBBBNexus/MonsterMass/States/MonsterStateData.h"

#include "MonsterPresentationComponent.generated.h"

class UAnimSequenceBase;

/** 向小怪动画蓝图提供只读 Mass 表现状态 */
UCLASS(ClassGroup = "Monster", BlueprintType, meta = (BlueprintSpawnableComponent))
class ABBB_EVAC_API UMonsterPresentationComponent final : public UActorComponent
{
    GENERATED_BODY()

public:
    /** 创建小怪表现状态组件 */
    UMonsterPresentationComponent();

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
    void ApplyPresentationState(
        EMonsterState InState,
        float InSpeed,
        float InStateTime,
        uint32 InActionId,
        float InActionProgress);

    /** @return 当前小怪状态 */
    UFUNCTION(BlueprintPure, Category = "Monster|Presentation")
    EMonsterState GetMonsterState() const;

    /** @return 当前移动速度 */
    UFUNCTION(BlueprintPure, Category = "Monster|Presentation")
    float GetMovementSpeed() const;

    /** @return 当前状态进入时间 */
    UFUNCTION(BlueprintPure, Category = "Monster|Presentation")
    float GetStateEnteredTime() const;

private:
    /** 根据逻辑状态选择表现动画 */
    UAnimSequenceBase* GetAnimationForState(EMonsterState InState) const;

    /** 待机状态循环动画 */
    UPROPERTY(EditAnywhere, Category = "Monster|Animation")
    TObjectPtr<UAnimSequenceBase> IdleAnimation;

    /** 侦察状态循环动画 */
    UPROPERTY(EditAnywhere, Category = "Monster|Animation")
    TObjectPtr<UAnimSequenceBase> ScoutAnimation;

    /** 追击状态循环动画 */
    UPROPERTY(EditAnywhere, Category = "Monster|Animation")
    TObjectPtr<UAnimSequenceBase> ChaseAnimation;

    /** 攻击状态动画 */
    UPROPERTY(EditAnywhere, Category = "Monster|Animation")
    TObjectPtr<UAnimSequenceBase> AttackAnimation;

    /** 受伤状态动画 */
    UPROPERTY(EditAnywhere, Category = "Monster|Animation")
    TObjectPtr<UAnimSequenceBase> HurtAnimation;

    /** 死亡状态动画 */
    UPROPERTY(EditAnywhere, Category = "Monster|Animation")
    TObjectPtr<UAnimSequenceBase> DeadAnimation;

    /** 当前逻辑状态 */
    UPROPERTY(Transient)
    EMonsterState MonsterState = EMonsterState::Idle;

    /** 当前移动速度 */
    UPROPERTY(Transient)
    float MovementSpeed = 0.0f;

    /** 当前状态进入时间 */
    UPROPERTY(Transient)
    float StateEnteredTime = 0.0f;

    /** 上一次已播放动画对应的状态 */
    EMonsterState LastPlayedState = EMonsterState::Idle;

    /** 是否已经向网格应用过动画 */
    bool bHasAppliedAnimation = false;

    /** 上一次已应用的逻辑动作编号 */
    uint32 LastActionId = 0;
};
