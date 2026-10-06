#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "BBBMonsterBloodImpact.h"
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

    /** @return 释放世界表现组件 无返回值 */
    virtual void Deinitialize() override;

private:
    /** 受上限约束的血迹组件 */
    UPROPERTY(Transient)
    TArray<TObjectPtr<UDecalComponent>> Decals;
    /** 池中组件最近显示时间 仅为视觉覆盖策略 */
    TArray<double> DecalTimes;
};
