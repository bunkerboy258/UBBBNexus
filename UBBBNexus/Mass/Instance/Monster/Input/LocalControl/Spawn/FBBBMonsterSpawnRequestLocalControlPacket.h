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
    /** 独立生成入口使用零阶段 */
    int32 StageIndex = 0;
    /** 唯一出生规则 */
    TWeakObjectPtr<UBBBMonsterSpawnDefinition> Definition;
    /** 本请求冻结的世界区域 */
    TArray<FBBBMonsterSpawnRegionDefinition> Regions;
    /** 与网络身份无关的非零种子 */
    uint32 Seed = 1;
    /** 本请求最多实际创建的数量 */
    int32 Count = 1;

    /** @return 输入值是否完整有效 */
    bool IsValid() const
    {
        FString Error;
        if (!OwnerId.IsValid() || RequestId == 0 || Generation == 0 || StageIndex < 0 || Seed == 0
            || Count < 1 || Count > 512 || !Definition.IsValid() || !Definition->Validate(Error) || Regions.IsEmpty())
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
        State.StageIndex = StageIndex;
        State.Definition = Definition;
        State.Regions = Regions;
        State.Seed = Seed;
        State.Requested = Count;
        State.bActive = true;
        State.Reason = TEXT("正在选点");
    }
};
