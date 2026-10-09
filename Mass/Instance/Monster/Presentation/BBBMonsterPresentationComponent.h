#pragma once

#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Behavior/BBBMonsterBehavior.h"
#include "Components/ActorComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitReactionFragment.h"

#include "BBBMonsterPresentationComponent.generated.h"

struct FBBBMonsterVariationFragment;


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

    /**
     * 复制当前踉跄事实 不推进计时或决定移动
     * @param bActive	Mass 当前站立踉跄是否成立
     * @param Progress	Mass 当前动作归一化进度
     * @param Region		本段动作固定使用的命中部位
     * @return 无返回值
     */
    UFUNCTION()
    void ApplyStaggerState(bool bActive, float Progress, EBBBMonsterHitRegion Region);

    /**
     * @param Variation	实体固定出生属性
     * @param InLoopPhase	Mass 当前循环相位
     * @return 无
     */
    void ApplyVariationFacts(const FBBBMonsterVariationFragment& Variation, float InLoopPhase);

    /**
     * 从当前骨骼姿势启用本地死亡物理 不贡献玩法事实
     * @param Velocity		死亡时的移动惯性
     * @param Hit		最近命中表现
     * @param Now		当前世界时间
     * @param bSimulate		本轮是否获得活动物理预算
     * @return 是否成功创建物理表现
     */
    bool BeginCorpsePresentation(const FVector& Velocity, const FBBBMonsterHitReactionFragment& Hit, float Now, bool bSimulate = true);

    /**
     * 从 Mass 批量调度检查物理落地与休眠 不创建组件 Tick
     * @param Now		当前世界时间
     * @param MaximumDuration	最长活动物理时长
     * @return 无
     */
    void UpdateCorpsePresentation(float Now, float MaximumDuration);

    /** @return 是否已消费当前代实体的死亡表现 */
    bool IsCorpseActive() const { return bCorpseActive; }

    /** @return 当前是否占用活动布娃娃预算 */
    bool IsCorpseSimulating() const { return bCorpseActive && !bCorpseFrozen; }

    /** @return 当前物理开始时间 */
    float GetCorpseStartedAt() const { return CorpseStartedAt; }

    /** @return 停止持续物理并保留当前骨骼姿势 无返回值 */
    void FreezeCorpsePresentation();

    /** @return 复用表现对象前清除尸体状态 无返回值 */
    void ResetCorpsePresentation();

private:
    /** 等待动画图消费当前事实的引擎帧 不记录玩法历史 */
    uint64 CorpsePoseRequestFrame = MAX_uint64;

    /** 当前代实体的本地死亡物理已经初始化 */
    bool bCorpseActive = false;

    /** 已保留最终姿势 不再模拟刚体 */
    bool bCorpseFrozen = false;

    /** 本地物理开始时间 不维护玩法死亡计时 */
    float CorpseStartedAt = -1.0f;

    /** 死亡前网格挂接变换 仅用于池化复用 */
    FTransform LivingMeshRelativeTransform = FTransform::Identity;

    /** 冻结前连续低速时长 */
    float CorpseStableSince = -1.0f;

    /** 已绑定实体的固定出生种子 */
    uint32 VariationSeed = 0;

    /** 固定移动风格 */
    int32 LocomotionStyle = 0;

    /** 实际速度到基础动画速度的换算比例 */
    float AnimationSpeedScale = 1.0f;

    /** 只读当前循环相位 */
    float LoopPhase = 0.0f;

    /** Mass 最近命中只读快照 */
    FBBBMonsterHitReactionFragment HitReaction;

    /** Mass 持续爬行结果的只读快照 */
    bool bCrawling = false;

    /** Mass 爬行姿态过渡的只读进度 */
    float CrawlProgress = 0.0f;

    /** 当前踉跄只读快照 */
    bool bStaggering = false;

    /** 当前踉跄归一化进度 */
    float StaggerProgress = 0.0f;

    /** 当前踉跄的固定部位 */
    EBBBMonsterHitRegion StaggerRegion = EBBBMonsterHitRegion::Torso;

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
