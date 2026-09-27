#pragma once

#include "GameFramework/Actor.h"
#include "MassEntityTypes.h"
#include "Mass/EntityHandle.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Network/BBBMonsterReplicationArray.h"
#include "BBBMassNetworkActor.generated.h"

/** 世界级 Mass 当前结果传输载体 */
UCLASS(NotBlueprintable)
class ABBB_EVAC_API ABBBMassNetworkActor final : public AActor
{
    GENERATED_BODY()

public:
    ABBBMassNetworkActor();
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    /** 开始本轮存在性观察 */
    void BeginPublish();

    /**
     * @param State	已由领域生成的事实
     * @param Entity	对应本机实体
     * @return 无
     */
    void Publish(const FBBBMonsterReplicationItem& State, FMassEntityHandle Entity);

    /** 移除本轮已不存在的复制项 */
    void EndPublish();

    /**
     * @param InstanceId	跨机实例身份
     * @return 本机对应句柄
     */
    FMassEntityHandle FindEntity(const FGuid& InstanceId) const;

    /**
     * @param InstanceId	本机实体身份
     * @param Health	本机已经成立的剩余生命
     * @return 无
     */
    void ObserveLocalHealth(const FGuid& InstanceId, float Health);

    /** 重送尚未出现在房主结果中的当前生命事实 */
    void SendLocalHealth();

private:
    UFUNCTION()
    void OnRep_Monsters();

    UPROPERTY(ReplicatedUsing = OnRep_Monsters)
    FBBBMonsterReplicationArray Monsters;

    TMap<FGuid, FMassEntityHandle> LocalEntities;
    TMap<FGuid, int32> ReplicationIndices;
    TSet<FGuid> Observed;
    TMap<FGuid, float> LocalHealth;
    TSet<FGuid> Retired;
};
