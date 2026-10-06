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
