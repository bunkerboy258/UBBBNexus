#pragma once

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehavior.h"
#include "Components/ActorComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitReactionFragment.h"

#include "BBBMonsterPresentationComponent.generated.h"


/** 向小怪动画蓝图提供只读 Mass 表现状态 */
UCLASS(ClassGroup = "Monster", BlueprintType, meta = (BlueprintSpawnableComponent))
class ABBB_EVAC_API UBBBMonsterPresentationComponent final : public UActorComponent
{
    GENERATED_BODY()

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

    /**
     * @param Hit	最近命中事实
     * @return 无返回值
     */
    void ApplyHitReaction(const FBBBMonsterHitReactionFragment& Hit);

    /**
     * 复制 Mass 姿态与几何事实 不参与爬行裁决
     * @param bInCrawling	本代实体是否持续爬行
     * @param InProgress	转入爬行归一化进度
     * @param InHalfHeight	当前逻辑胶囊半高
     * @return 无
     */
    void ApplyMobilityState(bool bInCrawling, float InProgress, float InHalfHeight);

private:
    /** Mass 最近命中只读快照 */
    FBBBMonsterHitReactionFragment HitReaction;

    /** Mass 持续爬行结果的只读快照 */
    bool bCrawling = false;

    /** Mass 爬行姿态过渡的只读进度 */
    float CrawlProgress = 0.0f;
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
