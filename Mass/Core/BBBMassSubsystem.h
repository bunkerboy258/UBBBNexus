#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "MassEntityManager.h"
#include "MassEntitySubsystem.h"
#include "Engine/World.h"
#include "BBBWork/UBBBNexus/Mass/Core/BBBMassCollisionBody.h"
#include "BBBMassSubsystem.generated.h"

class UMassEntityConfigAsset;
struct FBBBProjectileSpawnLocalControlPacket;
struct FBBBMonsterDamageLocalControlPacket;
struct FBBBMonsterDamageRemoteMessagePacket;
struct FBBBMonsterDamageContribution;
struct FBBBMonsterStateAuthorityFactPacket;

/** 世界级实体生命周期与输入路由 */
UCLASS()
class ABBB_EVAC_API UBBBMassSubsystem final : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    /**
     * @param Config	实体模板配置
     * @return 创建的实体句柄
     */
    FMassEntityHandle CreateEntity(const UMassEntityConfigAsset& Config);

    /** 清空本帧逻辑碰撞缓存 */
    void BeginCollisionFrame();

    /**
     * @param Body	当前球体快照
     * @return 无
     */
    void AddCollisionBody(const FBBBMassCollisionBody& Body);

    /**
     * @param Start	扫掠起点
     * @param End	扫掠终点
     * @param Radius	扫掠半径
     * @param Ignored	本次扫掠忽略的实体
     * @param HitEntity	最近命中实体
     * @param HitTime	线段归一化命中时间
     * @return 是否命中逻辑球体
     */
    bool TraceEntities(const FVector& Start, const FVector& End, float Radius,
        TConstArrayView<FMassEntityHandle> Ignored, FMassEntityHandle& HitEntity, float& HitTime) const;

    /**
     * 查询包含尚未消费输入的累计贡献快照 不暴露目标 Fragment
     * @param Entity	目标小怪实体
     * @param Result	已应用与待解析的累计贡献 当前结果按玩家身份排序
     * @return 是否取得有效目标的快照
     */
    bool QueryDamage(FMassEntityHandle Entity, TArray<FBBBMonsterDamageContribution>& Result) const;

    /**
     * @param Entity	目标实体
     * @param Packet	覆盖提交的输入
     * @return 是否成功投递
     */
    template<typename TPacket>
    bool SubmitInput(FMassEntityHandle Entity, TPacket&& Packet)
    {
        return RouteInput(Entity, Forward<TPacket>(Packet));
    }

private:
    bool RouteInput(FMassEntityHandle Entity, const FBBBProjectileSpawnLocalControlPacket& Packet);
    bool RouteInput(FMassEntityHandle Entity, const FBBBMonsterDamageLocalControlPacket& Packet);
    bool RouteInput(FMassEntityHandle Entity, const FBBBMonsterDamageRemoteMessagePacket& Packet);
    bool RouteInput(FMassEntityHandle Entity, const FBBBMonsterStateAuthorityFactPacket& Packet);

    /** 每帧重建的空间桶 不持有实体玩法状态 */
    TMap<FIntVector, TArray<FBBBMassCollisionBody>> CollisionCells;
};
