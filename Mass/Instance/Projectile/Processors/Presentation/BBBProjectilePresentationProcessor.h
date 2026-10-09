#pragma once

#include "MassProcessor.h"
#include "MassEntityQuery.h"
#include "BBBProjectilePresentationProcessor.generated.h"

class UNiagaraComponent;
class UNiagaraDataChannelAsset;
class UNiagaraSystem;
class UStaticMesh;
class UInstancedStaticMeshComponent;

/** 按通道批量发布子弹光效事实 */
UCLASS()
class ABBB_EVAC_API UBBBProjectilePresentationProcessor final : public UMassProcessor
{
    GENERATED_BODY()

public:
    /** 创建批量处理器 */
    UBBBProjectilePresentationProcessor();

protected:
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    /** 每种弹体网格共用一个批量组件 */
    UPROPERTY(Transient)
    TMap<TObjectPtr<UStaticMesh>, TObjectPtr<UInstancedStaticMeshComponent>> MeshComponents;

    FMassEntityQuery EntityQuery;

    /** 当前世界唯一的共享子弹光效组件 */
    UPROPERTY(Transient)
    TObjectPtr<UNiagaraComponent> SystemComponent;

    /** 可重用的表现数据槽位 */
    TArray<int32> FreeSlots;

    /** 上一帧结束的槽位 在光效读取消亡标记后重用 */
    TArray<int32> PendingReleaseSlots;

    /** 尚未分配过的下一个槽位 */
    int32 NextSlot = 0;

    /** 当前世界唯一的子弹表现通道 */
    TWeakObjectPtr<UNiagaraDataChannelAsset> ActiveChannel;

    /** 当前世界唯一的子弹表现系统 */
    TWeakObjectPtr<UNiagaraSystem> ActiveSystem;

};
