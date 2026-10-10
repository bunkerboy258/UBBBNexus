#pragma once

#include "Components/ActorComponent.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterSeveredPartDefinition.h"
#include "BBBMonsterSeveringPresentationComponent.generated.h"

class UStaticMeshComponent;
class USkeletalMeshComponent;
class UBBBMonsterDefinition;
class UMaterialInterface;
struct FBBBMonsterHealthFragment;
struct FBBBMonsterHitReactionFragment;

/** 仅还原已成立的部位损毁 不裁决伤害或新增逐实例逻辑更新 */
UCLASS()
class ABBB_EVAC_API UBBBMonsterSeveringPresentationComponent final : public UActorComponent
{
    GENERATED_BODY()

public:
    /** @return 创建无组件更新的断肢表现 */
    UBBBMonsterSeveringPresentationComponent();

    /**
     * @param Health	当前贡献推导的六部位受损程度与损毁结果
     * @param Definition	当前外观的共享伤口资源
     * @param Hit	最近有效命中 过期时不生成一次性表现
     * @param Seed	本代稳定出生种子
     * @param bNewActor	是否刚还原当前快照 新载体不重播断肢过程
     * @return 无返回值
     */
    void ApplyWoundFacts(const FBBBMonsterHealthFragment& Health, const UBBBMonsterDefinition& Definition,
        const FBBBMonsterHitReactionFragment& Hit, uint32 Seed, bool bNewActor);

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

    /** 只复用一个蒙皮伤口载体 不负责世界掉落物 */
    UPROPERTY(Transient)
    TObjectPtr<USkeletalMeshComponent> Wounds;

    /** 池化复用时恢复完整外观的共享材质 */
    UPROPERTY(Transient)
    TArray<TObjectPtr<UMaterialInterface>> IntactMaterials;

    /** 六部位当前材质阶段 仅用于跳过没有改变的表现写入 */
    TArray<float> AppliedStages;
};
