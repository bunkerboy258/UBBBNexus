#pragma once
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorSpawnFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDirectorDefinition.h"

struct FBBBMonsterDirectorInputFragment;

/** 按实际出生数量结算当前预留额度 */
struct FBBBMonsterDirectorCreatedLocalControlPacket final
{
    using FInputFragment = FBBBMonsterDirectorInputFragment;

    /** 目标导演身份 */
    FGuid InstanceId;
    /** 目标运行代号 */
    uint32 Generation = 0;
    /** 目标阶段 */
    int32 StageIndex = INDEX_NONE;
    /** 当前申请身份 */
    uint32 RequestId = 0;
    /** 实际成功创建数量 */
    int32 Count = 0;
    /** 创建结束的世界时间 */
    double Time = 0.0;
    /** 当前创建结果的原因 */
    FString Reason;

    /** @return 输入数据是否有效 */
    bool IsValid() const
    {
        return InstanceId.IsValid() && Generation > 0 && StageIndex >= 0 && RequestId > 0 && Count >= 0 && FMath::IsFinite(Time) && Time >= 0.0;
    }

    /**
     * @param State	当前导演状态
     * @param Spawn	当前出生申请
     * @return 当前输入能否应用
     */
    bool CanApply(const FBBBMonsterDirectorFragment& State, const FBBBMonsterDirectorSpawnFragment& Spawn) const
    {
        const auto* Definition = State.Definition.Get();
        return Definition && Definition->Stages.IsValidIndex(StageIndex)
            && State.InstanceId == InstanceId && State.Generation == Generation && State.StageIndex == StageIndex
            && State.Phase == EBBBMonsterDirectorPhase::Spawning && Spawn.RequestId == RequestId && Spawn.Reserved > 0 && Spawn.bCreating && Count <= Spawn.Reserved;
    }

    /**
     * @param State	当前导演状态
     * @param Spawn	当前出生申请
     * @return 无返回值
     */
    void Apply(FBBBMonsterDirectorFragment& State, FBBBMonsterDirectorSpawnFragment& Spawn) const
    {
        State.StageSpawned += Count;
        State.TotalSpawned += Count;
        const auto* Definition = State.Definition.Get();
        const float Interval = Count > 0 ? Definition->Stages[StageIndex].Interval : Definition->RetryInterval;
        State.NextSpawnTime = Time + Interval;
        State.Reason = Reason;
        ++State.Revision;
        Spawn.Reserved = 0;
        Spawn.bCreating = false;
        Spawn.bResultReady = false;
        Spawn.Locations.Reset();
    }
};
