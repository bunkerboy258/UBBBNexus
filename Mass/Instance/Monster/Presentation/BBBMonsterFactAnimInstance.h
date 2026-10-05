#pragma once

#include "Animation/AnimInstance.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehavior.h"

#include "BBBMonsterFactAnimInstance.generated.h"

class UBBBMonsterPresentationComponent;

/** 为事实驱动动画蓝图复制 Mass 快照 不选择资产或维护播放器 */
UCLASS(Transient, Blueprintable)
class ABBB_EVAC_API UBBBMonsterFactAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    //~ Begin UAnimInstance Interface
    /** @return 缓存所属表现组件 无返回值 */
    virtual void NativeInitializeAnimation() override;

    /**
     * 在游戏线程复制事实 供动画图线程只读
     * @param DeltaSeconds	引擎更新间隔 不推进逻辑进度
     * @return 无返回值
     */
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;
    //~ End UAnimInstance Interface

private:
    /** 本地表现对象身份 仅供蓝图错开姿势 不作为网络实体身份 */
    UPROPERTY(Transient, BlueprintReadOnly, Category = "小怪|事实", meta = (AllowPrivateAccess = "true", DisplayName = "表现标识事实"))
    int64 PresentationIdFact = 0;

    /** Mass 当前行为 */
    UPROPERTY(Transient, BlueprintReadOnly, Category = "小怪|事实", meta = (AllowPrivateAccess = "true", DisplayName = "行为事实"))
    EBBBMonsterBehavior BehaviorFact = EBBBMonsterBehavior::Idle;

    /** 实际水平速度 厘米每秒 */
    UPROPERTY(Transient, BlueprintReadOnly, Category = "小怪|事实", meta = (AllowPrivateAccess = "true", DisplayName = "移动速度事实"))
    float MovementSpeedFact = 0.0f;

    /** 当前非循环动作归一化进度 */
    UPROPERTY(Transient, BlueprintReadOnly, Category = "小怪|事实", meta = (AllowPrivateAccess = "true", DisplayName = "动作进度事实"))
    float ActionProgressFact = 0.0f;

    /** Mass 动作编号 同状态新动作也改变 */
    UPROPERTY(Transient, BlueprintReadOnly, Category = "小怪|事实", meta = (AllowPrivateAccess = "true", DisplayName = "动作标识事实"))
    int64 ActionIdFact = 0;

    /** Mass 状态进入时间 */
    UPROPERTY(Transient, BlueprintReadOnly, Category = "小怪|事实", meta = (AllowPrivateAccess = "true", DisplayName = "状态进入时间事实"))
    float StateEnteredTimeFact = 0.0f;

    /** 只在初始化时查找 不持有玩法状态 */
    UPROPERTY(Transient)
    TWeakObjectPtr<UBBBMonsterPresentationComponent> Presentation;
};
