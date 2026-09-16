#pragma once

#include "CoreMinimal.h"

class ABBBEquipmentInstance;
struct FBBBEquipmentActionResult;

/** 装备挂接与人物装备表现系统 */
class ABBB_EVAC_API FBBBEquipmentEquipSystem final
{
public:
    bool Activate(ABBBEquipmentInstance &Instance) const;
    void Deactivate(ABBBEquipmentInstance &Instance) const;
    bool BeginAction(
        ABBBEquipmentInstance &Instance,
        int32 Sequence,
        float DurationOverride,
        FBBBEquipmentActionResult &OutResult) const;
};
