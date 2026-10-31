#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "NavigationData.h"
#include "BBBMonsterNavigationCondition.h"
#include "BBBMonsterNavigationFragment.generated.h"

/** 导航路径与本次移动目的地 仅由寻路处理器维护 */
USTRUCT()
struct ABBB_EVAC_API FBBBMonsterNavigationFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 有效导航路径的完整拐点 */
    UPROPERTY()
    TArray<FVector> PathPoints;

    /** 各拐点到路径末端的长度 */
    UPROPERTY()
    TArray<float> TailDistances;

    /** 保留导航失效通知 由 Mass 在预算内重算 禁止原生自动重算 */
    FNavPathSharedPtr NativePath;

    /** 当前应接近的拐点 */
    int32 PathPointIndex = 1;

    /** 本次巡逻或追击目的地 */
    FVector Destination = FVector::ZeroVector;

    /** 当前路径关联的行为编号 */
    uint32 ActionId = MAX_uint32;

    /** 下次允许刷新导航路径的时间 */
    float NextPathRefreshTime = 0.0f;

    /** 上次成功查询时的原始目标位置 */
    FVector RequestedGoal = FVector::ZeroVector;

    /** 上次成功接受路线的时间 */
    float LastPathTime = -1.0f;

    /** 本次待处理请求开始等待的时间 未等待时为负 */
    float RequestSince = -1.0f;

    /** 最近一次 APS 同步调用耗时 单位为毫秒 */
    float LastQueryMs = 0.0f;

    /** 最近一次实际提交的弧线强度 用于调试 */
    float ArcStrength = 0.0f;

    /** 最近一次实际提交的横向偏移 用于调试 */
    float LaneOffset = 0.0f;

    /** 缓存路径是否可用于移动 */
    bool bHasPath = false;

    /** 当前巡逻目的地是否已到达 */
    bool bReachedDestination = false;

    /** 当前路径是否只到达可达边界 */
    bool bPartialPath = false;

    /** 路径使用的目标版本 */
    uint32 TargetRevision = MAX_uint32;

    /** 寻路失败或无进展的连续次数 */
    int32 FailureCount = 0;

    /** 此位置暂时不可达的结束时刻 */
    float ExcludedUntil = 0.0f;

    /** 暂时不可达的位置 */
    FVector ExcludedPosition = FVector::ZeroVector;

    /** 上次有实际移动进展的世界时间 */
    float ProgressTime = -1.0f;

    /** 当前安全前瞻目标 使用导航脚底坐标 */
    FVector FollowTarget = FVector::ZeroVector;

    /** 前瞻目标已经通过导航和当前身体净空检查 */
    bool bHasFollowTarget = false;

    /** 最近一次局部检查受阻 下一次成功检查前保持该原因 */
    bool bLocalBlocked = false;

    /** 缓存前瞻目标所在路线段 经过该段后必须重新检查 */
    int32 FollowSegmentIndex = 0;

    /** 上次构建路线时的爬行状态 姿态改变必须重新检查通行 */
    bool bCrawlingPath = false;

    /** 当前路线剩余长度 不以玩家直线距离代替 */
    float RemainingDistance = 0.0f;

    /** 连续路线上的净推进量 刷新路线只更换剩余距离基准 不清空推进计时 */
    float ProgressDistance = 0.0f;

    /** 当前进展窗口曾被同伴压缩通行空间 不能据此排除玩家位置 */
    bool bProgressCrowded = false;

    /** 身体脚底到当前路线段的偏移距离 */
    float OffPathDistance = 0.0f;

    /** 下一次允许刷新局部前瞻的时刻 */
    float NextFollowRefreshTime = 0.0f;

    /** 当前局部绕行方向 短时保持一侧避免左右反复切换 */
    int8 AvoidanceSide = 0;

    /** 允许重新选择局部绕行侧的时刻 */
    float AvoidanceSideUntil = 0.0f;

    /** 最近一次前瞻目标方向 仅用于统计转向反复 */
    FVector LastFollowDirection = FVector::ZeroVector;

    /** 路线执行期间方向反复的累计次数 */
    uint32 DirectionReversals = 0;

    /** 该实体已提交的重新寻路次数 */
    uint32 RepathCount = 0;

    /** 最近路线长度与同条件基础路线长度的比值 零表示未抽样 */
    float DetourRatio = 0.0f;

    /** 允许再次抽样基础路线的时刻 */
    float NextReferenceTime = 0.0f;

    /** 当前实际位移与移动意图的比例 */
    float MotionEfficiency = 1.0f;

    /** 有移动意图却持续受阻的累计秒数 换路线不抹掉真实阻塞 */
    float BlockedDuration = 0.0f;

    /** 当前路线情况 与寻路成功返回值分开记录 */
    EBBBMonsterNavigationCondition Condition = EBBBMonsterNavigationCondition::WaitingForPath;

    /** 当前请求或最近一次重新寻路的直接原因 不保存重算历史 */
    EBBBMonsterNavigationCondition LastRepathCondition = EBBBMonsterNavigationCondition::WaitingForPath;

    /** 当前稳定的包围接近位置编号 */
    int32 ApproachSlot = INDEX_NONE;

    /** 无实际推进时接近位置的释放时刻 攻击期间保持占位 */
    float ApproachExpiresAt = 0.0f;

    /** 包围位置所属的玩家 不读取隐藏玩家位置 */
    TWeakObjectPtr<AActor> ApproachActor;

    /** 因所有近战接近位置拥堵而合理等待 */
    bool bWaitingForSpace = false;
};

/** 完整导航路径使用原生构造 拷贝与析构 */
template<>
struct TMassFragmentTraits<FBBBMonsterNavigationFragment>
{
    enum
    {
        AuthorAcceptsItsNotTriviallyCopyable = true
    };
};
