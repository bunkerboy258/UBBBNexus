#pragma once
#include "MassEntityTypes.h"
#include "BBBMonsterDirectorSpawnFragment.generated.h"

/** 每个导演仅保留一个当前出生申请 */
USTRUCT()
struct ABBB_EVAC_API FBBBMonsterDirectorSpawnFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 申请身份 取消时递增 */
    uint32 RequestId = 0;
    /** 当前预留数量 */
    int32 Reserved = 0;
    /** 当前引擎查询身份 */
    int32 QueryId = INDEX_NONE;
    /** 查询启动时间 */
    double StartedAt = 0.0;
    /** 尚未启动查询的申请 */
    bool bNeedsQuery = false;
    /** 当前查询已交回结果 */
    bool bResultReady = false;
    /** 当前已排入安全创建时机 */
    bool bCreating = false;
    /** 当前筛选后的出生位置 */
    TArray<FVector> Locations;
};

/** 当前申请与配置副本需要原生构造 拷贝和析构 */
template<>
struct TMassFragmentTraits<FBBBMonsterDirectorSpawnFragment>
{
    enum
    {
        AuthorAcceptsItsNotTriviallyCopyable = true
    };
};
