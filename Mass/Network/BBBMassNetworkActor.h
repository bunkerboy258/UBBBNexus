#pragma once

#include "GameFramework/Actor.h"
#include "MassEntityTypes.h"
#include "Mass/EntityHandle.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Network/BBBMonsterReplicationArray.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Input/RemoteMessage/Health/FBBBMonsterDamageTotalsRemoteMessagePacket.h"
#include "BBBMassPendingDamageSnapshot.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Slime/Fragments/BBBSlimeFact.h"
#include "BBBMassNetworkActor.generated.h"

/** 世界级 Mass 当前结果传输载体 不执行生命裁决 */
UCLASS(NotBlueprintable)
class ABBB_EVAC_API ABBBMassNetworkActor final : public AActor
{
    GENERATED_BODY()

public:
    ABBBMassNetworkActor();
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    /** @param Reason	载体结束原因 @return 无 仅回收本载体在非权威端创建的镜像实体 */
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;

    /** @return 原生 FastArray 当前小怪快照的只读视图 */
    TConstArrayView<FBBBMonsterReplicationItem> ReadMonsterFacts() const
    {
        return Monsters.Items;
    }

    /** @return 当前粘液复制快照的只读视图 */
    TConstArrayView<FBBBSlimeFact> ReadSlimeFacts() const
    {
        return Slimes;
    }

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
    /** @param States 当前全部粘液结果 @return 仅发布当前快照 */
    void PublishSlimes(const TArray<FBBBSlimeFact>& States);
    FMassEntityHandle FindEntity(const FGuid& InstanceId) const;

    /**
     * 仅累计值增加时发送
     * @param InstanceId	目标小怪这一代的身份
     * @param PlayerId	本机玩家身份 必须与连接所属玩家一致
     * @param CumulativeDamage	本机玩家对目标的最新累计伤害
     * @return 是否已交接可靠通道
     */
    bool ReportLocalDamage(const FGuid& InstanceId, const FBBBMonsterDamageTotals& Contribution);

private:
    UFUNCTION()
    void OnRep_Slimes();

    UPROPERTY(ReplicatedUsing = OnRep_Slimes)
    TArray<FBBBSlimeFact> Slimes;

    /** 当前快照的本机粘液身份映射 */
    TMap<FGuid, FMassEntityHandle> LocalSlimes;
    UFUNCTION()
    void OnRep_Monsters();

    /** 当前字典快照的可靠投递 不另设死亡消息或命中事件链路 */
    UFUNCTION(NetMulticast, Reliable)
    void MulticastDamage(FGuid InstanceId, const TArray<FBBBMonsterDamageTotals>& Contributions);

    void RouteDamage(const FGuid& InstanceId, const TArray<FBBBMonsterDamageTotals>& Contributions, bool bInitialSnapshot = false);

    /** 清理已经离开当前复制集合的暂存与退役身份 不撤销现存实体的退役保护 */
    void PruneCaches();

    UPROPERTY(ReplicatedUsing = OnRep_Monsters)
    FBBBMonsterReplicationArray Monsters;

    TMap<FGuid, FMassEntityHandle> LocalEntities;
    TMap<FGuid, int32> ReplicationIndices;
    TSet<FGuid> Observed;
    TMap<FGuid, FBBBMonsterDamageTotals> SubmittedDamage;

    /** 可靠字典早于出生属性到达时暂存当前结果 不保存消息历史 */
    TMap<FGuid, FBBBMassPendingDamageSnapshot> PendingDamageSnapshots;
    TMap<FGuid, float> Retired;

    /** 有网络活动时至多每秒整理一次缓存 */
    float NextCachePruneTime = 0.0f;
};
