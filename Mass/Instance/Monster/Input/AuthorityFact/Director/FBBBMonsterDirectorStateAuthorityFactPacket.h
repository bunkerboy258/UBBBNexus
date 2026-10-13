#pragma once
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Network/BBBMonsterDirectorReplicationState.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorSpawnFragment.h"

struct FBBBMonsterDirectorInputFragment;

/** 将主机当前导演结果还原到客机镜像实体 */
struct FBBBMonsterDirectorStateAuthorityFactPacket final
{
    using FInputFragment = FBBBMonsterDirectorInputFragment;
    /** 主机当前结果 */
    FBBBMonsterDirectorReplicationState State;
    /** 镜像显示使用的静态定义 */
    TWeakObjectPtr<UBBBMonsterDirectorDefinition> Definition;
    /** 当前镜像地图入口 */
    TWeakObjectPtr<ABBBMonsterDirectorPresentationActor> Presentation;

    /** @return 当前结果是否有效 */
    bool IsValid() const
    {
        return State.InstanceId.IsValid() && State.Revision > 0 && State.Alive >= 0 && State.Pending >= 0
            && State.TotalSpawned >= 0 && State.StageSpawned >= 0;
    }

    /**
     * @param Current	当前镜像状态
     * @param Spawn	当前镜像出生数量
     * @return 旧版本不能覆盖新结果
     */
    bool CanApply(const FBBBMonsterDirectorFragment& Current, const FBBBMonsterDirectorSpawnFragment& Spawn) const
    {
        return (Current.Revision == 0 || Current.InstanceId == State.InstanceId) && State.Revision > Current.Revision;
    }

    /**
     * @param Current	当前镜像状态
     * @param Spawn	当前镜像出生数量
     * @return 无返回值
     */
    void Apply(FBBBMonsterDirectorFragment& Current, FBBBMonsterDirectorSpawnFragment& Spawn) const
    {
        Current.InstanceId = State.InstanceId;
        Current.Definition = Definition;
        Current.Presentation = Presentation;
        Current.Generation = State.Generation;
        Current.Revision = State.Revision;
        Current.StageIndex = State.StageIndex;
        Current.Phase = State.Phase;
        Current.TotalSpawned = State.TotalSpawned;
        Current.StageSpawned = State.StageSpawned;
        Current.Alive = State.Alive;
        Current.Reason = State.Reason;
        Spawn.Reserved = State.Pending;
    }
};
