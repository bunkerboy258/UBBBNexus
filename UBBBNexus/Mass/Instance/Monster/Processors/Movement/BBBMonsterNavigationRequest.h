#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "NavigationData.h"
#include "BBBMonsterNavigationRequest.generated.h"

class ARecastNavMesh;
class UBBBMonsterDefinition;

/** 一次原生异步查询的不可变参数与回执 不直接访问实体 */
UCLASS(Transient)
class UBBBMonsterNavigationRequest final : public UObject
{
    GENERATED_BODY()
public:
    /** 发起查询的完整实体身份 */
    FMassEntityHandle Entity;
    /** 查询时的行为身份 */
    uint32 ActionId = 0;
    /** 查询时的目标版本 */
    uint32 TargetRevision = 0;
    /** 原生请求编号 */
    uint32 QueryId = 0;
    /** 当前请求独占的跨障能力 提交后不再修改 */
    float TraversalLimit = 0;
    /** 查询对应的爬行姿态 */
    bool bCrawling = false;
    /** 查询是否允许追击用部分路径 */
    bool bChase = false;
    /** 发起时的导航起点 */
    FVector Start = FVector::ZeroVector;
    /** 投影后的目标点 */
    FVector End = FVector::ZeroVector;
    /** 投影前的目标点 */
    FVector Goal = FVector::ZeroVector;
    /** 发起时的业务目标位置 */
    FVector RequestedGoal = FVector::ZeroVector;
    /** 当前请求使用的导航数据 */
    TWeakObjectPtr<ARecastNavMesh> NavData;
    /** 当前请求使用的规则 */
    TWeakObjectPtr<const UBBBMonsterDefinition> Definition;
    /** 独立过滤器 保持到处理器消费结果 */
    FSharedConstNavQueryFilter Filter;
    /** 原生查询回执 */
    FNavPathSharedPtr ResultPath;
    /** 原生查询结果 */
    ENavigationQueryResult::Type Result = ENavigationQueryResult::Invalid;
    /** 开始排队的单调时间 */
    double StartedAt = 0;
    /** 收到回执的单调时间 */
    double CompletedAt = 0;
    /** 回调只写本请求的回执 */
    bool bComplete = false;
};
