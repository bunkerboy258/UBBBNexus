#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"

#include "BBBMonsterMovementFragment.generated.h"

/** 小怪移动配置与运行状态 */
USTRUCT(BlueprintType)
struct ABBB_EVAC_API FBBBMonsterMovementFragment final : public FMassFragment
{
    GENERATED_BODY()

    /** 最大移动速度 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "小怪", meta = (DisplayName = "移动速度"))
    float MoveSpeed = 300.0f;

    /** 进入攻击状态的停止半径 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "小怪", meta = (DisplayName = "停止半径"))
    float StopRadius = 120.0f;

    /** 下次允许刷新导航路径的时间 */
    float NextPathRefreshTime = 0.0f;

    /** 当前导航路径上的下一点 */
    FVector NextPathPoint = FVector::ZeroVector;
};
