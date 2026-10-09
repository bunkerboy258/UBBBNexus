#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
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

    /** 当前应接近的拐点 */
    int32 PathPointIndex = 1;

    /** 本次巡逻或追击目的地 */
    FVector Destination = FVector::ZeroVector;

    /** 当前路径关联的行为编号 */
    uint32 ActionId = MAX_uint32;

    /** 下次允许刷新导航路径的时间 */
    float NextPathRefreshTime = 0.0f;

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

    /** 上次有实际移动进展的位置 */
    FVector ProgressPosition = FVector::ZeroVector;

    /** 上次有实际移动进展的世界时间 */
    float ProgressTime = -1.0f;

    /** 本次局部绕行结束时刻 */
    float DetourEndsAt = 0.0f;

    /** 本次局部绕行的位置 */
    FVector DetourPosition = FVector::ZeroVector;

    /** 当前稳定的包围接近位置编号 */
    int32 ApproachSlot = INDEX_NONE;

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
