#pragma once

#include "GameFramework/Actor.h"
#include "MassEntityTypes.h"
#include "Mass/EntityHandle.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Network/BBBMonsterReplicationArray.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/RemoteMessage/Health/FBBBMonsterDamageRemoteMessagePacket.h"
#include "BBBMassNetworkActor.generated.h"

/** 世界级 Mass 当前结果传输载体 不执行生命裁决 */
UCLASS(NotBlueprintable)
class ABBB_EVAC_API ABBBMassNetworkActor final : public AActor
{
    GENERATED_BODY()

public:
    ABBBMassNetworkActor();
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    void BeginPublish();

    /**
     * 伤害变化立即交接可靠传输 行动结果按观察器频率更新
     * @param State	当前行动与累计贡献快照
     * @param Entity	本机实体句柄
     * @param bUpdateAction	本轮是否发布行动结果 不影响伤害交接
     * @return 无
     */
    void Publish(const FBBBMonsterReplicationItem& State, FMassEntityHandle Entity, bool bUpdateAction = true);

    void EndPublish();
    FMassEntityHandle FindEntity(const FGuid& InstanceId) const;

    /**
     * 仅累计值增加时发送
     * @param InstanceId	目标小怪这一代的身份
     * @param PlayerId	本机玩家身份 必须与连接所属玩家一致
     * @param CumulativeDamage	本机玩家对目标的最新累计伤害
     * @return 是否已交接可靠通道
     */
    bool ReportLocalDamage(const FGuid& InstanceId, const FBBBMonsterDamageContribution& Contribution);

private:
    UFUNCTION()
    void OnRep_Monsters();

    /** 当前字典快照的可靠投递 不另设死亡消息或命中事件链路 */
    UFUNCTION(NetMulticast, Reliable)
    void MulticastDamage(FGuid InstanceId, const TArray<FBBBMonsterDamageContribution>& Contributions);

    void RouteDamage(const FGuid& InstanceId, const TArray<FBBBMonsterDamageContribution>& Contributions);

    UPROPERTY(ReplicatedUsing = OnRep_Monsters)
    FBBBMonsterReplicationArray Monsters;

    TMap<FGuid, FMassEntityHandle> LocalEntities;
    TMap<FGuid, int32> ReplicationIndices;
    TSet<FGuid> Observed;
    TMap<FGuid, double> SubmittedDamage;

    /** 可靠字典早于出生属性到达时暂存当前结果 不保存消息历史 */
    TMap<FGuid, FBBBMonsterDamageRemoteMessagePacket> PendingDamageSnapshots;
    TSet<FGuid> Retired;
};
