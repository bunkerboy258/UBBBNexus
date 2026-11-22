#pragma once
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Spawn/BBBMonsterSpawnFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterSpawnDefinition.h"

struct FBBBMonsterSpawnInputFragment;

/** 向指定入口提交一个当前出生请求 */
struct FBBBMonsterSpawnRequestLocalControlPacket final
{
    using FInputFragment = FBBBMonsterSpawnInputFragment;
    /** 生成入口的稳定归属身份 */
    FGuid OwnerId;
    /** 同一入口递增的请求号 */
    uint32 RequestId = 0;
    /** 当前运行代号 */
    uint32 Generation = 1;
    /** 本次请求的调度来源 */
    EBBBMonsterSpawnSource Source = EBBBMonsterSpawnSource::Spawner;
    /** 预制波次标识 非预制波次为 None */
    FName EventId;
    /** 唯一出生规则 */
    TWeakObjectPtr<UBBBMonsterSpawnDefinition> Definition;
    /** 本请求冻结的可刷世界区域 */
    TArray<FBBBMonsterSpawnRegionDefinition> Regions;
    /** 本请求冻结的禁刷世界区域 */
    TArray<FBBBMonsterSpawnRegionDefinition> ExcludeRegions;
    /** 与网络身份无关的非零种子 */
    uint32 Seed = 1;
    /** 本次出生可选的初始目标玩家编号 */
    int32 InitialTargetPlayerId = INDEX_NONE;
    /** 出生时是否强制追击玩家 */
    bool bAlertOnSpawn = false;
    /** 本请求最多实际创建的数量 */
    int32 Count = 1;

    /** @return 输入值是否完整有效 */
    bool IsValid() const
    {
        FString Error;
        if (!OwnerId.IsValid() || RequestId == 0 || Generation == 0 || Seed == 0
            || InitialTargetPlayerId < INDEX_NONE || Count < 1 || Count > 512
            || !Definition.IsValid() || !Definition->Validate(Error) || Regions.IsEmpty())
        {
            return false;
        }
        for (const auto& Region : Regions)
        {
            if (!Region.IsValid())
            {
                return false;
            }
        }
        for (const auto& Exclude : ExcludeRegions)
        {
            if (!Exclude.IsValid())
            {
                return false;
            }
        }
        return true;
    }

    /**
     * @param State	当前出生申请
     * @return 旧请求是否已经完成或取消
     */
    bool CanApply(const FBBBMonsterSpawnFragment& State) const
    {
        return !State.bActive && !State.bCreating
            && (!State.OwnerId.IsValid() || (State.OwnerId == OwnerId && RequestId > State.RequestId));
    }

    /**
     * @param State	当前出生申请
     * @return 无返回值
     */
    void Apply(FBBBMonsterSpawnFragment& State) const
    {
        State = FBBBMonsterSpawnFragment();
        State.OwnerId = OwnerId;
        State.RequestId = RequestId;
        State.Generation = Generation;
        State.Source = Source;
        State.EventId = EventId;
        State.Definition = Definition;
        State.Regions = Regions;
        State.ExcludeRegions = ExcludeRegions;
        State.Seed = Seed;
        State.InitialTargetPlayerId = InitialTargetPlayerId;
        State.bAlertOnSpawn = bAlertOnSpawn;
        State.Requested = Count;
        State.bActive = true;
        State.Reason = TEXT("正在选点");
    }
};
