#pragma once

#include "Components/ActorComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterSeveredPartDefinition.h"
#include "BBBMonsterSeveringPresentationComponent.generated.h"

class UStaticMeshComponent;
class USkeletalMeshComponent;

/** 仅还原已成立的部位损毁 不裁决伤害或新增逐实例逻辑更新 */
UCLASS()
class ABBB_EVAC_API UBBBMonsterSeveringPresentationComponent final : public UActorComponent
{
    GENERATED_BODY()

public:
    /** @return 创建无组件更新的断肢表现 */
    UBBBMonsterSeveringPresentationComponent();

    /**
     * @param DestroyedParts	Mass 推导出的本代部位损毁位图
     * @param Definitions	对应外观的已封口静态资源
     * @param ImpulseDirection	最近命中方向
     * @return 无返回值
     */
    void ApplyDestroyedParts(uint8 DestroyedParts, const TArray<FBBBMonsterSeveredPartDefinition>& Definitions, const FVector& ImpulseDirection);

    /** @return 批量调度掉落部件的休眠与回收 无返回值 */
    void UpdateDetachedParts();

    /** @return 池化复用前清除封口和隐藏骨骼 无返回值 */
    void ResetPresentation();

    /**
     * @param EndPlayReason	引擎销毁原因
     * @return 清除本对象的临时组件 无返回值
     */
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    /** 已经应用的只读损毁表现位图 */
    uint8 AppliedParts = 0;

    /** 本对象创建的断口与掉落组件 */
    UPROPERTY(Transient)
    TArray<TObjectPtr<UStaticMeshComponent>> Components;

    /** 本代需要复原的骨骼 */
    TArray<FName> HiddenBones;
};
