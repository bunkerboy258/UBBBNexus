#pragma once
#include "MassEntityTypes.h"
#include "BBBWork/UBBBNexus/Mass/Instance/Monster/Config/BBBMonsterSpawnSource.h"
#include "BBBMonsterDirectorSpawnFragment.generated.h"

/** 每个导演仅保留一个正在执行中的下游刷怪申请 */
USTRUCT()
struct ABBB_EVAC_API FBBBMonsterDirectorSpawnFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 申请身份 每次新请求或超时递增 */
    uint32 RequestId = 0;
    /** 当前申请的来源分类 */
    EBBBMonsterSpawnSource Source = EBBBMonsterSpawnSource::Population;
    /** 当前申请对应的事件标识 */
    FName EventId;
    /** 当前预留数量 */
    int32 Reserved = 0;
    /** 查询启动时间 */
    double StartedAt = 0.0;
    /** 已结算的当前请求实际创建数量 */
    int32 Accounted = 0;
};

template<>
struct TMassFragmentTraits<FBBBMonsterDirectorSpawnFragment>
{
    enum
    {
        AuthorAcceptsItsNotTriviallyCopyable = true
    };
};
