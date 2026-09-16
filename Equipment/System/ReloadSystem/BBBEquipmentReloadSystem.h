#pragma once

#include "CoreMinimal.h"

class ABBBEquipmentInstance;
struct FBBBEquipmentActionResult;

/** 换弹持续状态与人物表现系统 */
class ABBB_EVAC_API FBBBEquipmentReloadSystem final
{
public:
    bool Begin(
        ABBBEquipmentInstance &Instance,
        float WorldTimeSeconds,
        int32 Sequence,
        float DurationOverride,
        FBBBEquipmentActionResult &OutResult) const;

    void Advance(ABBBEquipmentInstance &Instance, float WorldTimeSeconds) const;
};
