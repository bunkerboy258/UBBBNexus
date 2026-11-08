#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "NavigationData.h"
#include "BBBMonsterNavigationCondition.h"
#include "BBBMonsterNavigationFragment.generated.h"

class dtPathCorridor;
class dtNavMeshQuery;

/** 导航处理器维护路线 移动处理器回传实际脚底位置 */
USTRUCT()
struct ABBB_EVAC_API FBBBMonsterNavigationFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 原生路径负责导航失效通知 */
    FNavPathSharedPtr NativePath;
    /** Detour 当前多边形走廊 */
    TSharedPtr<dtPathCorridor> Corridor;
    /** 同一世界顺序复用的查询器 不持有实体状态 */
    TSharedPtr<dtNavMeshQuery> Query;
    /** 本条路线使用的通行过滤器 */
    FSharedConstNavQueryFilter Filter;
    /** 当前前视拐点 上限六个 */
    TArray<FVector> Corners;
    /** 当前邻近导航墙边 成对保存端点 */
    TArray<FVector> Walls;
    /** 当前脚底前视目标 */
    FVector FollowTarget = FVector::ZeroVector;
    /** 当前可达终点 */
    FVector Destination = FVector::ZeroVector;
    /** 上次完整查询的原始可达终点 */
    FVector QueryDestination = FVector::ZeroVector;
    /** 上次查询对应的目标位置 */
    FVector RequestedGoal = FVector::ZeroVector;
    /** 最近一次实际推进的位置 */
    FVector ProgressPosition = FVector::ZeroVector;
    /** 待执行的跨障多边形 */
    NavNodeRef TraversalPoly = 0;
    /** 待执行的跨障连接 */
    FNavLinkId TraversalLink = FNavLinkId::Invalid;
    /** 当前跨障入口脚底 */
    FVector TraversalEntry = FVector::ZeroVector;
    /** 当前跨障出口脚底 */
    FVector TraversalExit = FVector::ZeroVector;
    /** 路线使用的行为身份 */
    uint32 ActionId = MAX_uint32;
    /** 路线使用的目标身份 */
    uint32 TargetRevision = MAX_uint32;
    /** 下次完整寻路时刻 */
    float NextPathRefreshTime = 0;
    /** 下次墙边缓存刷新时刻 */
    float NextBoundaryTime = 0;
    /** 上次成功查询时刻 */
    float LastPathTime = -1;
    /** 当前请求进入等待的时刻 */
    float RequestSince = -1;
    /** 最近异步请求从排队到回执的毫秒数 不表示主线程耗时 */
    float LastQueryMs = 0;
    /** 暂时不可达位置的重试时刻 */
    float ExcludedUntil = 0;
    /** 暂时不可达的位置 */
    FVector ExcludedPosition = FVector::ZeroVector;
    /** 最近实际推进时刻 */
    float ProgressTime = -1;
    /** 当前剩余路线长度 */
    float RemainingDistance = 0;
    /** 当前实际位移与请求位移比例 */
    float MotionEfficiency = 1;
    /** 环境阻挡持续时间 */
    float BlockedDuration = 0;
    /** 完整查询连续失败次数 */
    int32 FailureCount = 0;
    /** 完整查询次数 */
    uint32 RepathCount = 0;
    /** 当前路线可用于前进 */
    bool bHasPath = false;
    /** 当前目的地已到达 */
    bool bReachedDestination = false;
    /** 当前只能到达目标附近边界 */
    bool bPartialPath = false;
    /** 该路线使用爬行身体 */
    bool bCrawlingPath = false;
    /** 持续拥挤开始时刻 */
    float CrowdSince = -1;
    /** 连续拥堵后的接近方向尝试编号 */
    uint32 ApproachAttempt = 0;
    /** 当前停止或限制的直接原因 */
    FName StopReason;
    /** 当前路线执行情况 */
    EBBBMonsterNavigationCondition Condition = EBBBMonsterNavigationCondition::WaitingForPath;
    /** 最近重算原因 */
    EBBBMonsterNavigationCondition LastRepathCondition = EBBBMonsterNavigationCondition::WaitingForPath;
};

template<>
struct TMassFragmentTraits<FBBBMonsterNavigationFragment>
{
    enum { AuthorAcceptsItsNotTriviallyCopyable = true };
};
