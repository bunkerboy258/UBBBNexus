#pragma once

#include "HitReact.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/HitReaction/BBBMonsterHitReactionFragment.h"
#include "BBBMonsterHitReactionComponent.generated.h"

/** 只消费命中事实的骨骼受力表现 不维护伤害和移动状态 */
UCLASS(ClassGroup = "Monster", meta = (BlueprintSpawnableComponent))
class ABBB_EVAC_API UBBBMonsterHitReactionComponent final : public UHitReact
{
    GENERATED_BODY()

public:
    UBBBMonsterHitReactionComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    /**
     * 将最近命中事实交给局部物理表现 同一编号只消费一次
     * @param Hit	最近成立的命中
     * @param bAlive	当前存活事实
     * @return 无返回值
     */
    void ApplyHitFacts(const FBBBMonsterHitReactionFragment& Hit, bool bAlive, bool bAuthoredStagger);

    /**
     * 清除表现对象上一代实体的全部受力状态
     * @return 无返回值
     */
    void ResetPresentation();

    /** @return 当前已经消费的命中编号 */
    uint32 GetObservedHitSerial() const;

    //~ Begin UActorComponent Interface
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    //~ End UActorComponent Interface

    //~ Begin UHitReact Interface
    virtual bool CanHitReact_Implementation() const override;
    //~ End UHitReact Interface

protected:
    virtual void ResetHitReactSystem() override;

private:
#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR
    /** 仅自动化验收构造局部受力边界 不增加运行时编辑入口 */
    friend class FBBBMonsterHitReactionRefreshTest;
#endif

    /** 已消费的本地表现编号 与血效消费记录相互独立 */
    UPROPERTY(Transient, VisibleInstanceOnly, Category = "小怪|受击", meta = (DisplayName = "已消费命中编号"))
    uint32 ObservedHitSerial = 0;

    /** 本轮已经施加物理反应 结束时必须完成清理 */
    bool bHasAppliedReaction = false;

    /** 最近接收的存活事实 */
    bool bPresentationAlive = true;

    /** 只在活动受击期间请求完整骨骼更新 */
    void RequestAnimationUpdate() const;
};
