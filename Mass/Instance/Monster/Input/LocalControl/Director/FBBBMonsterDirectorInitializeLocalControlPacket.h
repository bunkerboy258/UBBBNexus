#pragma once
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Fragments/Director/BBBMonsterDirectorSpawnFragment.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterDirectorDefinition.h"

struct FBBBMonsterDirectorInputFragment;

/** 初始化导演的配置与地图绑定 */
struct FBBBMonsterDirectorInitializeLocalControlPacket final
{
    using FInputFragment = FBBBMonsterDirectorInputFragment;

    /** 本地图的导演身份 */
    FGuid InstanceId;
    /** 静态副本规则 */
    TWeakObjectPtr<UBBBMonsterDirectorDefinition> Definition;
    /** 地图桥接对象 */
    TWeakObjectPtr<ABBBMonsterDirectorPresentationActor> Presentation;
    /** 已检查的区域纯值快照 */
    TArray<FBBBMonsterSpawnRegionDefinition> Regions;

    /** @return 输入数据是否有效 */
    bool IsValid() const
    {
        FString Error;
        if (!InstanceId.IsValid() || !Definition.IsValid() || !Presentation.IsValid() || !Definition->Validate(Error) || Regions.IsEmpty())
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
        for (const auto& Stage : Definition->Stages)
        {
            for (const FName Id : Stage.RegionIds)
            {
                if (!Seen.Contains(Id))
                {
                    return false;
                }
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
        return !State.InstanceId.IsValid() && Spawn.Reserved == 0;
    }

    /**
     * @param State	当前导演状态
     * @param Spawn	当前出生申请
     * @return 无返回值
     */
    void Apply(FBBBMonsterDirectorFragment& State, FBBBMonsterDirectorSpawnFragment& Spawn) const
    {
        State.InstanceId = InstanceId;
        State.Definition = Definition;
        State.Presentation = Presentation;
        State.Regions = Regions;
        State.Reason = TEXT("配置完成 等待开始");
        ++State.Revision;
    }
};
