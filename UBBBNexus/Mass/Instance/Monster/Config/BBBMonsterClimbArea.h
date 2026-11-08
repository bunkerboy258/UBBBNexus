#pragma once

#include "NavAreas/NavArea.h"
#include "BBBMonsterClimbArea.generated.h"

/** 可以按当前肢体能力从查询中排除的上台连接 */
UCLASS()
class ABBB_EVAC_API UBBBMonsterClimbArea final : public UNavArea
{
    GENERATED_BODY()

public:
    UBBBMonsterClimbArea()
    {
        DefaultCost = 1.5f;
        FixedAreaEnteringCost = 250.0f;
        DrawColor = FColor(255, 140, 30);
    }
};
