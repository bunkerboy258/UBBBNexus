#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "BBBMonsterBloodImpact.h"
#include "BBBMonsterBloodDroplet.h"
#include "BBBMonsterBloodResidue.h"
#include "BBBMonsterBloodPresentationSubsystem.generated.h"

class UBBBMonsterBloodPresentationDefinition;
class UDecalComponent;

/** 世界共享血迹组件池 不创建逐命中玩法 Actor 或 Tick */
UCLASS()
class ABBB_EVAC_API UBBBMonsterBloodPresentationSubsystem final : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    /**
     * @param Settings	共享静态表现配置
     * @param Impacts	本轮接触事实
     * @return 无返回值
     */
    void Publish(const UBBBMonsterBloodPresentationDefinition& Settings, TConstArrayView<FBBBMonsterBloodImpact> Impacts);

    /**
     * 由 Mass 表现阶段统一推进世界血滴 碰撞后留下血迹
     * @param DeltaSeconds	本轮间隔
     * @return 无返回值
     */
    void AdvancePresentation(float DeltaSeconds);

    /** @return 释放世界表现组件 无返回值 */
    virtual void Deinitialize() override;

private:
    friend class FBBBMonsterBloodResidueTest;
    friend class UBBBBloodResidueEditorLibrary;

    /**
     * @param Settings	共享静态配置
     * @param Impact	当前有效命中
     * @return 无返回值
     */
    void EmitDroplets(const UBBBMonsterBloodPresentationDefinition& Settings, const FBBBMonsterBloodImpact& Impact);

    /**
     * @param Droplet	当前落地血滴
     * @param Contact	环境接触
     * @param TraceBudget	剩余边缘检查预算
     * @return 无返回值
     */
    void PlaceResidue(const FBBBMonsterBloodDroplet& Droplet, const FHitResult& Contact, int32& TraceBudget);

    /** 受上限约束的血迹组件 */
    UPROPERTY(Transient)
    TArray<TObjectPtr<UDecalComponent>> Decals;
    /** 与组件一一对应的有效表面状态 */
    TArray<FBBBMonsterBloodResidue> Residues;

    /** 有上限的当前飞行表现 */
    TArray<FBBBMonsterBloodDroplet> Droplets;

    /** 本轮实际消耗的环境查询 */
    int32 LastTraceCount = 0;

    /** 本轮推进的处理耗时 */
    double LastAdvanceMilliseconds = 0.0;
};
