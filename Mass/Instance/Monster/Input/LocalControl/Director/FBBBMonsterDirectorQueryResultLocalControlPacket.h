#pragma once
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorSpawnFragment.h"

struct FBBBMonsterDirectorInputFragment;

/** 交回当前出生查询的纯值结果 */
struct FBBBMonsterDirectorQueryResultLocalControlPacket final
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
    /** 引擎查询给出的当前位置 */
    TArray<FVector> Locations;

    /** @return 输入数据是否有效 */
    bool IsValid() const
    {
        if (!InstanceId.IsValid() || Generation == 0 || StageIndex < 0 || RequestId == 0 || Locations.Num() > 512)
        {
            return false;
        }
        for (const FVector& Location : Locations)
        {
            if (Location.ContainsNaN())
            {
                return false;
            }
        }
        return true;
    }

    /**
     * @param State	当前导演状态
     * @param Spawn	当前出生申请
     * @return 当前输入能否应用
     */
    bool CanApply(const FBBBMonsterDirectorFragment& State, const FBBBMonsterDirectorSpawnFragment& Spawn) const
    {
        return State.InstanceId == InstanceId && State.Generation == Generation && State.StageIndex == StageIndex
            && State.Phase == EBBBMonsterDirectorPhase::Spawning && Spawn.RequestId == RequestId && Spawn.Reserved > 0 && !Spawn.bCreating;
    }

    /**
     * @param State	当前导演状态
     * @param Spawn	当前出生申请
     * @return 无返回值
     */
    void Apply(FBBBMonsterDirectorFragment& State, FBBBMonsterDirectorSpawnFragment& Spawn) const
    {
        Spawn.Locations = Locations;
        Spawn.bResultReady = true;
        Spawn.QueryId = INDEX_NONE;
    }
};
