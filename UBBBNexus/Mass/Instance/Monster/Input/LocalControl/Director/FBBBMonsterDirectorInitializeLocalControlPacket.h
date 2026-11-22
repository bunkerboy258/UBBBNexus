#pragma once
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDirectorDefinition.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorSpawnFragment.h"

struct FBBBMonsterDirectorInputFragment;

/** 为新创建的导演实体写入地图绑定与静态规则 */
struct FBBBMonsterDirectorInitializeLocalControlPacket final
{
    using FInputFragment = FBBBMonsterDirectorInputFragment;
    /** 导演实例身份 */
    FGuid InstanceId;
    /** 当前入口的随机根种子 */
    uint32 Seed = 1;
    /** 共享静态规则 */
    TWeakObjectPtr<UBBBMonsterDirectorDefinition> Definition;
    /** 地图桥接入口 */
    TWeakObjectPtr<ABBBMonsterDirectorPresentationActor> Presentation;
    /** 冻结的地图区域 包含固有区 事件区与禁刷区 */
    TArray<FBBBMonsterSpawnRegionDefinition> Regions;

    /** @return 初始化数据是否完整有效 */
    bool IsValid() const
    {
        FString Error;
        if (!InstanceId.IsValid() || Seed == 0 || !Definition.IsValid() || !Definition->Validate(Error))
        {
            return false;
        }
        TSet<FName> Seen;
        for (const auto& Region : Regions)
        {
            if (!Region.IsValid() || Seen.Contains(Region.Id))
            {
                return false;
            }
            Seen.Add(Region.Id);
        }
        return true;
    }

    /**
     * @param State	当前导演运行状态
     * @param Spawn	当前导演刷怪申请
     * @return 实体是否尚未初始化
     */
    bool CanApply(const FBBBMonsterDirectorFragment& State, const FBBBMonsterDirectorSpawnFragment&) const
    {
        return !State.InstanceId.IsValid();
    }

    /**
     * @param State	当前导演运行状态
     * @param Spawn	当前导演刷怪申请
     * @return 无返回值
     */
    void Apply(FBBBMonsterDirectorFragment& State, FBBBMonsterDirectorSpawnFragment& Spawn) const
    {
        State = FBBBMonsterDirectorFragment();
        Spawn = FBBBMonsterDirectorSpawnFragment();
        State.InstanceId = InstanceId;
        State.Seed = Seed;
        State.Definition = Definition;
        State.Presentation = Presentation;
        for (const auto& Region : Regions)
        {
            if (Region.Role == EBBBMonsterSpawnRegionRole::BlockSpawn)
            {
                State.ExcludeRegions.Add(Region);
            }
            else
            {
                State.Regions.Add(Region);
            }
        }
        State.Reason = TEXT("已初始化 等待开始");
    }
};
