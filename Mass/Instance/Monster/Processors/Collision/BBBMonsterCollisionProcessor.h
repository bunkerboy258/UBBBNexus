#pragma once

#include "MassProcessor.h"
#include "MassEntityQuery.h"
#include "BBBMonsterCollisionProcessor.generated.h"

struct FBBBMassCollisionBody;

/** 发布与表现无关的小怪逻辑碰撞快照 */
UCLASS()
class ABBB_EVAC_API UBBBMonsterCollisionProcessor final : public UMassProcessor
{
    GENERATED_BODY()

public:
    /** 创建批量处理器 */
    UBBBMonsterCollisionProcessor();

    /**
     * @param World 当前世界
     * @param Entity 完整代际句柄
     * @param Start 扫掠起点
     * @param End 扫掠终点
     * @param Radius 扫掠半径
     * @param HitBody 最近实际部位球体
     * @param HitTime 归一化接触时间
     * @return 当前活体部位是否相交 不保存跨帧骨骼或 Fragment 引用
     */
    static bool TraceCompound(UWorld& World, FMassEntityHandle Entity, const FVector& Start,
        const FVector& End, float Radius, FBBBMassCollisionBody& HitBody, float& HitTime);

    /**
     * @param World 当前世界
     * @param Entity 完整代际句柄
     * @param Center 范围中心
     * @param Radius 范围半径
     * @param HitBody 距离范围中心最近的实际部位
     * @return 当前活体部位是否与范围相交
     */
    static bool OverlapCompound(UWorld& World, FMassEntityHandle Entity, const FVector& Center,
        float Radius, FBBBMassCollisionBody& HitBody);

protected:
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    FMassEntityQuery EntityQuery;

};
