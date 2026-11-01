#pragma once

#include "GameFramework/Actor.h"
#include "BBBMassNavigationQueryActor.generated.h"

/** 单次同步寻路的能力参数载体 不持有实体状态 */
UCLASS(Transient, NotPlaceable, NotBlueprintable)
class ABBB_EVAC_API ABBBMassNavigationQueryActor final : public AActor
{
    GENERATED_BODY()

public:
    ABBBMassNavigationQueryActor();

    /** @param Height 本次允许的攀爬高度 零为禁止 @return 无返回值 */
    void SetTraversalLimit(float Height);

    /** @return 本次同步查询允许的攀爬高度 */
    float GetTraversalLimit() const { return MaximumHeight; }

private:
    /** 仅在一次同步查询期间有效 */
    float MaximumHeight = 0;
};
