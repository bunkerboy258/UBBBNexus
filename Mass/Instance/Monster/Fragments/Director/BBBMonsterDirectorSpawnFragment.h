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
    /** 查询启动时间 */
    double StartedAt = 0.0;
    /** 已结算的当前请求实际创建数量 */
    int32 Accounted = 0;
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
